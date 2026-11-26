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

#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

#include "hcl/register_interface.hpp"

namespace {

class MockTransport final : public hcl::RegisterTransport {
public:
    hcl::Status connect() override {
        state_.connected = true;
        return hcl::Status::success();
    }

    void disconnect() override {
        state_.connected = false;
    }

    bool is_connected() const override {
        return state_.connected;
    }

    hcl::Result<std::uint32_t> read_register(std::uint32_t base_address, std::uint32_t offset) override {
        if (!state_.connected) {
            return hcl::Result<std::uint32_t>::from_error(hcl::StatusCode::TransportError);
        }

        const std::uint32_t address = base_address + offset;
        return hcl::Result<std::uint32_t>::from_value(state_.registers[address & 0x3u]);
    }

    hcl::Status write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) override {
        if (!state_.connected) {
            return hcl::Status::error(hcl::StatusCode::TransportError);
        }

        const std::uint32_t address = base_address + offset;
        state_.registers[address & 0x3u] = value;
        return hcl::Status::success();
    }

    hcl::Status execute(hcl_register_operation* operations, std::size_t operation_count) override {
        if (!state_.connected) {
            return hcl::Status::error(hcl::StatusCode::TransportError);
        }
        if (operation_count > 0u && operations == nullptr) {
            return hcl::Status::error(hcl::StatusCode::InvalidArgument);
        }

        hcl::Status result = hcl::Status::success();
        for (std::size_t i = 0; i < operation_count; ++i) {
            auto& op = operations[i];
            const std::uint32_t address = op.base_address + op.offset;
            switch (op.operation) {
                case HCL_REGISTER_OP_READ32:
                    op.read_value = state_.registers[address & 0x3u];
                    op.status = HCL_STATUS_OK;
                    break;
                case HCL_REGISTER_OP_WRITE32:
                    state_.registers[address & 0x3u] = op.write_value;
                    op.status = HCL_STATUS_OK;
                    break;
                default:
                    op.status = HCL_STATUS_INVALID_ARGUMENT;
                    result = hcl::Status::error(hcl::StatusCode::InvalidArgument);
                    break;
            }
        }

        return result;
    }

private:
    struct {
        bool connected{false};
        std::uint32_t registers[4]{0u, 0u, 0u, 0u};
    } state_;
};

}  // namespace

int main() {
    auto transport = std::make_shared<MockTransport>();
    assert(transport->connect().ok());

    hcl::RegisterInterface access(transport);

    assert(access.write32(0x0u, 0x0u, 0xFFFF0000u).ok());

    assert(access.write32(0x0u, 0x0u, 0xFFFF00ABu).ok());
    const auto value = access.read32(0x0u, 0x0u);
    assert(value.ok());
    assert(value.value == 0xFFFF00ABu);

    std::vector<hcl::Operation> batch_ops;
    batch_ops.push_back({hcl::OperationType::Write32, 0x0u, 0x1u, 0xCAFEBABEu, 0u, hcl::StatusCode::Ok});
    batch_ops.push_back({hcl::OperationType::Read32, 0x0u, 0x1u, 0u, 0u, hcl::StatusCode::Ok});

    const auto batch_status = access.execute(batch_ops);
    assert(batch_status.ok());
    assert(batch_ops[0].status == hcl::StatusCode::Ok);
    assert(batch_ops[1].status == hcl::StatusCode::Ok);
    assert(batch_ops[1].read_value == 0xCAFEBABEu);

    std::vector<hcl::Operation> invalid_batch_ops;
    invalid_batch_ops.push_back({hcl::OperationType::Write32, 0x0u, 0x2u, 0x11111111u, 0u, hcl::StatusCode::Ok});
    invalid_batch_ops.push_back({static_cast<hcl::OperationType>(99), 0x0u, 0x3u, 0u, 0u, hcl::StatusCode::Ok});
    invalid_batch_ops.push_back({hcl::OperationType::Write32, 0x0u, 0x2u, 0x22222222u, 0u, hcl::StatusCode::Ok});

    const auto invalid_batch_status = access.execute(invalid_batch_ops);
    assert(!invalid_batch_status.ok());
    assert(invalid_batch_ops[0].status == hcl::StatusCode::Ok);
    assert(invalid_batch_ops[1].status == hcl::StatusCode::InvalidArgument);
    assert(invalid_batch_ops[2].status == hcl::StatusCode::Ok);

    const auto post_fail_fast = access.read32(0x0u, 0x2u);
    assert(post_fail_fast.ok());
    assert(post_fail_fast.value == 0x22222222u);

    const hcl::Status context_status = hcl::Status::error(hcl::StatusCode::InvalidArgument,
                                                          hcl::RegisterContext{0x10u, 0x20u, 0x30u});
    const hcl_status c_status = hcl::to_hcl_status(context_status);
    assert(c_status.code == HCL_STATUS_INVALID_ARGUMENT);
    assert(c_status.has_context);
    assert(c_status.ctx.base_address == 0x10u);
    assert(c_status.ctx.offset == 0x20u);
    assert(c_status.ctx.value == 0x30u);

    const hcl::Status round_trip = hcl::to_status(c_status);
    assert(!round_trip.ok());
    assert(round_trip.has_context);
    assert(round_trip.ctx.base_address == 0x10u);
    assert(round_trip.ctx.offset == 0x20u);
    assert(round_trip.ctx.value == 0x30u);

    return 0;
}