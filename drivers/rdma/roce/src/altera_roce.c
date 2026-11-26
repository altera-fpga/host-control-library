/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hcl/drivers/altera_roce.h"
#include "hcl/register_interface_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct hcl_altera_roce {
    hcl_register_interface* reg;
    uint32_t base_address;
};

static hcl_status write_reg(hcl_altera_roce* d, uint32_t offset, uint32_t value) {
    const hcl_status status = hcl_register_interface_write32(d->reg, d->base_address, offset, value);
    return status;
}

static hcl_status read_reg(hcl_altera_roce* d, uint32_t offset, uint32_t* out) {
    const hcl_status status = hcl_register_interface_read32(d->reg, d->base_address, offset, out);
    return status;
}

static hcl_status execute(hcl_altera_roce* d,
                          hcl_register_operation* ops,
                          size_t op_count) {
    const hcl_status status = hcl_register_interface_execute(d->reg, ops, op_count);
    return status;
}

static hcl_status wait_init_done(hcl_altera_roce* d) {
    hcl_deadline deadline;
    hcl_compute_deadline(1000u, &deadline);
    while (!hcl_deadline_exceeded(&deadline)) {
        uint32_t value = 0u;
        const hcl_status s = read_reg(d, HCL_ROCE_STATUS_REG_ADDR, &value);
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
        if ((value & HCL_ROCE_STATUS_INIT_DONE_BIT) != 0u) {
            return hcl_status_ok();
        }
        hcl_delay_ms(1u);
    }
    return hcl_status_error(HCL_STATUS_TIMEOUT, NULL);
}

static hcl_status poll_table_ctrl_busy(hcl_altera_roce* d) {
    hcl_deadline deadline;
    hcl_compute_deadline(1000u, &deadline);
    while (!hcl_deadline_exceeded(&deadline)) {
        uint32_t value = 0u;
        const hcl_status s = read_reg(d, HCL_ROCE_TABLE_CTRL_REG_ADDR, &value);
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
        if ((value & HCL_ROCE_TABLE_CTRL_BUSY_BIT) == 0u) {
            return hcl_status_ok();
        }
        hcl_delay_ms(1u);
    }
    return hcl_status_error(HCL_STATUS_TIMEOUT, NULL);
}

static hcl_status write_table(hcl_altera_roce* d,
                              uint32_t table_type,
                              uint32_t entry,
                              const uint32_t* words,
                              uint32_t num_words) {
    hcl_register_operation* ops;
    const uint32_t ctrl = (entry & 0x00FFFFFFu) | ((table_type & 0x3u) << 24u);
    const size_t op_count = (size_t)num_words + 1u;

    if (words == NULL && num_words > 0u) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    ops = (hcl_register_operation*)calloc(op_count, sizeof(*ops));
    if (ops == NULL) {
        return hcl_status_error(HCL_STATUS_ALLOCATION_FAILED, NULL);
    }

    for (uint32_t i = 0u; i < num_words; ++i) {
        ops[i].operation = HCL_REGISTER_OP_WRITE32;
        ops[i].base_address = d->base_address;
        ops[i].offset = HCL_ROCE_TABLE_DATA_REG_BASE_ADDR + (i * 4u);
        ops[i].write_value = words[i];
        ops[i].read_value = 0u;
        ops[i].status = HCL_STATUS_OK;
    }

    ops[num_words].operation = HCL_REGISTER_OP_WRITE32;
    ops[num_words].base_address = d->base_address;
    ops[num_words].offset = HCL_ROCE_TABLE_CTRL_REG_ADDR;
    ops[num_words].write_value = ctrl;
    ops[num_words].read_value = 0u;
    ops[num_words].status = HCL_STATUS_OK;

    const hcl_status s = execute(d, ops, op_count);
    free(ops);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return poll_table_ctrl_busy(d);
}

static void pack_qp_config(const hcl_roce_qp_config* cfg, uint32_t* w) {
    w[0] = ((cfg->rsvd_mode & 0x1Fu) << 27u)
         | ((cfg->mode      & 0x7u)  << 24u)
         | ( cfg->remote_qp & 0xFFFFFFu);
    w[1] = (cfg->src_hdr_table_entry & 0xFFFFFFu);
    w[2] = ((cfg->ipv6_valid & 0x1u) << 25u)
         | ((cfg->vlan_valid & 0x1u) << 24u)
         | ( cfg->dst_hdr_table_entry & 0xFFFFFFu);
    w[3] = ((cfg->read_ops_allowed & 0xFFu) << 24u)
         | ( cfg->initiator_psn    & 0xFFFFFFu);
    w[4] = ((cfg->timeout    & 0x1Fu) << 24u)
         | ( cfg->target_psn & 0xFFFFFFu);
    w[5] = ((cfg->rnr_en    & 0x1u) << 24u)
         | ( cfg->rqe_queue & 0xFFFFFFu);
}

static void pack_src_hdr(const hcl_roce_src_hdr* hdr, uint32_t* w) {
    w[0] = (uint32_t)( hdr->src_mac         & 0xFFFFFFFFu);
    w[1] = (uint32_t)((hdr->src_mac >> 32u) & 0xFFFFu);
    w[2] = (uint32_t)( hdr->src_ip_lo        & 0xFFFFFFFFu);
    w[3] = (uint32_t)((hdr->src_ip_lo >> 32u) & 0xFFFFFFFFu);
    w[4] = (uint32_t)( hdr->src_ip_hi        & 0xFFFFFFFFu);
    w[5] = (uint32_t)((hdr->src_ip_hi >> 32u) & 0xFFFFFFFFu);
}

static void pack_dst_hdr(const hcl_roce_dst_hdr* hdr, uint32_t* w) {
    w[0] = (uint32_t)( hdr->dst_mac         & 0xFFFFFFFFu);
    w[1] = (uint32_t)((hdr->dst_mac >> 32u) & 0xFFFFu)
         | ((hdr->vlan_vid & 0xFFFu)  << 16u)
         | ((hdr->vlan_dei & 0x1u)    << 28u)
         | ((hdr->vlan_pcp & 0x7u)    << 29u);
    w[2] = (uint32_t)( hdr->dst_ip_lo        & 0xFFFFFFFFu);
    w[3] = (uint32_t)((hdr->dst_ip_lo >> 32u) & 0xFFFFFFFFu);
    w[4] = (uint32_t)( hdr->dst_ip_hi        & 0xFFFFFFFFu);
    w[5] = (uint32_t)((hdr->dst_ip_hi >> 32u) & 0xFFFFFFFFu);
    w[6] = ( hdr->dscp         & 0x3Fu)
         | ((hdr->udp_src_port & 0xFFFFu) << 16u);
    w[7] = (hdr->pkey & 0xFFFFu);
    w[8] = hdr->qkey;
}

hcl_altera_roce* hcl_altera_roce_create(hcl_register_interface* reg,
                                        uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }
    hcl_altera_roce* d = (hcl_altera_roce*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }
    d->reg = reg;
    d->base_address = base_address;
    return d;
}

void hcl_altera_roce_destroy(hcl_altera_roce* driver) {
    free(driver);
}

hcl_status hcl_altera_roce_init_global(hcl_altera_roce* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    hcl_status s = wait_init_done(d);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_reg(d, HCL_ROCE_CONF0_REG_ADDR, HCL_ROCE_DEFAULT_LOG2_PAYLOAD_MTU & 0xFu);
}

hcl_status hcl_altera_roce_configure_qp_uc(hcl_altera_roce* d,
                                           const hcl_roce_uc_qp_params* params) {
    if (d == NULL || params == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    hcl_roce_qp_config cfg = {0};
    cfg.remote_qp           = params->remote_qp;
    cfg.mode                = HCL_ROCE_QP_MODE_UC;
    cfg.rsvd_mode           = 0u;
    cfg.src_hdr_table_entry = params->src_hdr_entry;
    cfg.dst_hdr_table_entry = params->dst_hdr_entry;
    cfg.vlan_valid          = params->vlan_valid;
    cfg.ipv6_valid          = params->ipv6_valid;
    cfg.initiator_psn       = params->initiator_psn;
    cfg.read_ops_allowed    = params->read_ops_allowed;
    cfg.target_psn          = params->target_psn;
    cfg.timeout             = params->timeout;
    cfg.rqe_queue           = params->rqe_queue;
    cfg.rnr_en              = params->rnr_en;

    uint32_t words[HCL_ROCE_QP_CONFIG_WORDS];
    pack_qp_config(&cfg, words);
    return write_table(d, HCL_ROCE_TABLE_QP_CONFIG, params->local_qp,
                       words, HCL_ROCE_QP_CONFIG_WORDS);
}

hcl_status hcl_altera_roce_configure_src_hdr(hcl_altera_roce* d,
                                             uint32_t entry,
                                             const hcl_roce_src_hdr* hdr) {
    if (d == NULL || hdr == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t words[HCL_ROCE_SRC_HDR_WORDS];
    pack_src_hdr(hdr, words);
    return write_table(d, HCL_ROCE_TABLE_SRC_HDR, entry,
                       words, HCL_ROCE_SRC_HDR_WORDS);
}

hcl_status hcl_altera_roce_configure_dst_hdr(hcl_altera_roce* d,
                                             uint32_t entry,
                                             const hcl_roce_dst_hdr* hdr) {
    if (d == NULL || hdr == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t words[HCL_ROCE_DST_HDR_WORDS];
    pack_dst_hdr(hdr, words);
    return write_table(d, HCL_ROCE_TABLE_DST_HDR, entry,
                       words, HCL_ROCE_DST_HDR_WORDS);
}

hcl_status hcl_altera_roce_init_qp_uc(hcl_altera_roce* d,
                                      const hcl_roce_uc_qp_params* params) {
    if (d == NULL || params == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    hcl_status s = hcl_altera_roce_configure_qp_uc(d, params);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    s = hcl_altera_roce_configure_src_hdr(d, params->src_hdr_entry, &params->src);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return hcl_altera_roce_configure_dst_hdr(d, params->dst_hdr_entry, &params->dst);
}
