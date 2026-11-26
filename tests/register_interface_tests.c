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

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hcl/register_interface.h"
#include "hcl/register_transport.h"

struct MockTransportState {
    bool connected;
    uint32_t registers[4];
    uint32_t read_calls;
    uint32_t write_calls;
    uint32_t batch_calls;
};

static hcl_status_code mock_connect(hcl_register_transport* transport) {
    struct MockTransportState* state = (struct MockTransportState*)transport->context;
    state->connected = true;
    return HCL_STATUS_OK;
}

static void mock_disconnect(hcl_register_transport* transport) {
    struct MockTransportState* state = (struct MockTransportState*)transport->context;
    state->connected = false;
}

static bool mock_is_connected(const hcl_register_transport* transport) {
    const struct MockTransportState* state = (const struct MockTransportState*)transport->context;
    return state->connected;
}

static hcl_status mock_read32(const hcl_register_transport* transport,
                              uint32_t base_address,
                              uint32_t offset,
                              uint32_t* out_value) {
    struct MockTransportState* state = (struct MockTransportState*)transport->context;
    if (!state->connected) {
        return hcl_status_error(HCL_STATUS_TRANSPORT_ERROR,
                                &(hcl_register_context){base_address, offset, 0u});
    }

    state->read_calls += 1u;
    const uint32_t address = base_address + offset;
    *out_value = state->registers[address & 0x3u];
    return hcl_status_ok();
}

static hcl_status mock_write32(const hcl_register_transport* transport,
                               uint32_t base_address,
                               uint32_t offset,
                               uint32_t value) {
    struct MockTransportState* state = (struct MockTransportState*)transport->context;
    if (!state->connected) {
        return hcl_status_error(HCL_STATUS_TRANSPORT_ERROR,
                                &(hcl_register_context){base_address, offset, value});
    }

    state->write_calls += 1u;
    const uint32_t address = base_address + offset;
    state->registers[address & 0x3u] = value;
    return hcl_status_ok();
}

static hcl_status mock_execute(const hcl_register_transport* transport,
                               hcl_register_operation* operations,
                               size_t operation_count) {
    struct MockTransportState* state = (struct MockTransportState*)transport->context;
    size_t index;
    hcl_status_code aggregate_status = HCL_STATUS_OK;

    if (state == NULL || (operation_count > 0u && operations == NULL)) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!state->connected) {
        return hcl_status_error(HCL_STATUS_TRANSPORT_ERROR, NULL);
    }

    state->batch_calls += 1u;

    for (index = 0u; index < operation_count; ++index) {
        hcl_status_code operation_status = HCL_STATUS_INVALID_ARGUMENT;
        const uint32_t address = operations[index].base_address + operations[index].offset;

        if (operations[index].operation == HCL_REGISTER_OP_READ32) {
            operations[index].read_value = state->registers[address & 0x3u];
            operation_status = HCL_STATUS_OK;
        } else if (operations[index].operation == HCL_REGISTER_OP_WRITE32) {
            state->registers[address & 0x3u] = operations[index].write_value;
            operation_status = HCL_STATUS_OK;
        }

        operations[index].status = operation_status;

        if (operation_status != HCL_STATUS_OK && aggregate_status == HCL_STATUS_OK) {
            aggregate_status = operation_status;
        }
    }

    return hcl_status_error(aggregate_status, NULL);
}

static const hcl_register_transport_interface mock_vtable = {
    &mock_connect,
    &mock_disconnect,
    &mock_is_connected,
    &mock_read32,
    &mock_write32,
    &mock_execute,
};

static const hcl_register_transport_interface mock_vtable_without_execute = {
    &mock_connect,
    &mock_disconnect,
    &mock_is_connected,
    &mock_read32,
    &mock_write32,
    NULL,
};

static hcl_register_transport make_mock_transport(void* context, bool include_execute_callback) {
    hcl_register_transport transport = {0};
    transport.context = context;
    transport.vtable = include_execute_callback ? &mock_vtable : &mock_vtable_without_execute;
    return transport;
}

int main(void) {
    struct MockTransportState state = {false, {0u, 0u, 0u, 0u}, 0u, 0u, 0u};

    const hcl_register_transport invalid_transport = make_mock_transport(&state, false);
    assert(hcl_register_interface_create(&invalid_transport) == NULL);

    hcl_register_interface* access = NULL;
    const hcl_register_transport valid_transport = make_mock_transport(&state, true);
    assert(hcl_register_transport_connect((hcl_register_transport*)&valid_transport) == HCL_STATUS_OK);

    access = hcl_register_interface_create(&valid_transport);
    assert(access != NULL);

    hcl_status write_status = hcl_register_interface_write32(access, 0x0u, 0x0u, 0xFFFF0000u);
    assert(hcl_status_is_ok(&write_status));

    write_status = hcl_register_interface_write32(access, 0x0u, 0x0u, 0xFFFF00ABu);
    assert(hcl_status_is_ok(&write_status));

    uint32_t value = 0u;
    hcl_status read_status = hcl_register_interface_read32(access, 0x0u, 0x0u, &value);
    assert(hcl_status_is_ok(&read_status));
    assert(value == 0xFFFF00ABu);

    hcl_register_operation fallback_batch[] = {
        {HCL_REGISTER_OP_WRITE32, 0x0u, 0x1u, 0xDEADBEEFu, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0x0u, 0x1u, 0u, 0u, HCL_STATUS_OK}
    };
    hcl_status batch_status = hcl_register_interface_execute(access,
                                                          fallback_batch,
                                                          2u);
    assert(hcl_status_is_ok(&batch_status));
    assert(fallback_batch[1].read_value == 0xDEADBEEFu);
    assert(state.batch_calls == 1u);

    hcl_register_operation invalid_fallback_batch[] = {
        {HCL_REGISTER_OP_WRITE32, 0x0u, 0x2u, 0xAAAAAAAAu, 0u, HCL_STATUS_OK},
        {(hcl_register_operation_type)99, 0x0u, 0x3u, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, 0x0u, 0x2u, 0xBBBBBBBBu, 0u, HCL_STATUS_OK}
    };
    batch_status = hcl_register_interface_execute(access,
                                                invalid_fallback_batch,
                                                3u);
    assert(batch_status.code == HCL_STATUS_INVALID_ARGUMENT);
    assert(invalid_fallback_batch[0].status == HCL_STATUS_OK);
    assert(invalid_fallback_batch[1].status == HCL_STATUS_INVALID_ARGUMENT);
    assert(invalid_fallback_batch[2].status == HCL_STATUS_OK);

    uint32_t post_fail_fast_value = 0u;
    read_status = hcl_register_interface_read32(access, 0x0u, 0x2u, &post_fail_fast_value);
    assert(hcl_status_is_ok(&read_status));
    assert(post_fail_fast_value == 0xBBBBBBBBu);

    hcl_register_interface_destroy(access);

    state.connected = false;
    state.registers[0] = 0u;
    state.registers[1] = 0u;
    state.registers[2] = 0u;
    state.registers[3] = 0u;
    state.read_calls = 0u;
    state.write_calls = 0u;
    state.batch_calls = 0u;

    const hcl_register_transport batch_ops = make_mock_transport(&state, true);
    assert(hcl_register_transport_connect((hcl_register_transport*)&batch_ops) == HCL_STATUS_OK);
    access = hcl_register_interface_create(&batch_ops);
    assert(access != NULL);

    hcl_register_operation native_batch[] = {
        {HCL_REGISTER_OP_WRITE32, 0x0u, 0x0u, 0x12345678u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0x0u, 0x0u, 0u, 0u, HCL_STATUS_OK}
    };
    batch_status = hcl_register_interface_execute(access,
                                               native_batch,
                                               2u);
    assert(hcl_status_is_ok(&batch_status));
    assert(native_batch[1].read_value == 0x12345678u);
    assert(state.batch_calls == 1u);

    hcl_register_interface_destroy(access);
    return 0;
}
