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

#pragma once

#include <stdint.h>

typedef struct hcl_roce_qp_config {
    uint32_t remote_qp;
    uint32_t mode;
    uint32_t rsvd_mode;
    uint32_t src_hdr_table_entry;
    uint32_t dst_hdr_table_entry;
    uint32_t vlan_valid;        /* 1-bit */
    uint32_t ipv6_valid;        /* 1-bit */
    uint32_t initiator_psn;     /* 24-bit */
    uint32_t read_ops_allowed;  /* 8-bit */
    uint32_t target_psn;        /* 24-bit */
    uint32_t timeout;           /* 5-bit */
    uint32_t rqe_queue;
    uint32_t rnr_en;            /* 1-bit */
} hcl_roce_qp_config;

typedef struct hcl_roce_src_hdr {
    uint64_t src_mac;    /* 48-bit MAC address */
    uint64_t src_ip_hi;  /* upper 64 bits of 128-bit IP address */
    uint64_t src_ip_lo;  /* lower 64 bits */
} hcl_roce_src_hdr;

typedef struct hcl_roce_dst_hdr {
    uint64_t dst_mac;       /* 48-bit MAC address */
    uint32_t vlan_vid;      /* 12-bit */
    uint32_t vlan_dei;      /* 1-bit */
    uint32_t vlan_pcp;      /* 3-bit */
    uint64_t dst_ip_hi;     /* upper 64 bits of 128-bit IP address */
    uint64_t dst_ip_lo;     /* lower 64 bits */
    uint32_t dscp;          /* 6-bit */
    uint32_t udp_src_port;  /* 16-bit */
    uint32_t pkey;          /* 16-bit */
    uint32_t qkey;
} hcl_roce_dst_hdr;

typedef struct hcl_roce_uc_qp_params {
    uint32_t local_qp;          /* QP config table entry to program */
    uint32_t remote_qp;
    uint32_t src_hdr_entry;
    uint32_t dst_hdr_entry;
    uint32_t rqe_queue;
    uint32_t initiator_psn;     /* 24-bit */
    uint32_t target_psn;        /* 24-bit */
    uint32_t timeout;           /* 5-bit */
    uint32_t rnr_en;            /* 1-bit */
    uint32_t vlan_valid;        /* 1-bit */
    uint32_t ipv6_valid;        /* 1-bit */
    uint32_t read_ops_allowed;  /* 8-bit */
    hcl_roce_src_hdr src;
    hcl_roce_dst_hdr dst;
} hcl_roce_uc_qp_params;
