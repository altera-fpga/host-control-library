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

#include <cstdint>
#include <string>

#include "hcl/register_transport.hpp"

struct hcl_uio_register_transport;

namespace hcl {

class UioRegisterTransport final : public RegisterTransport {
public:
    /**
     * Construct a UIO transport.
     *
     * @param device_path  Path to the UIO character device, e.g. @c /dev/uio0.
     * @param map_index    Sysfs map index to use (default 0).
     */
    explicit UioRegisterTransport(std::string device_path,
                                  unsigned int map_index = 0u);

    ~UioRegisterTransport() override;

    UioRegisterTransport(const UioRegisterTransport&) = delete;
    UioRegisterTransport& operator=(const UioRegisterTransport&) = delete;

    /**
     * Open the UIO device and mmap the memory region.
     *
     * Reads the map size from
     * @c /sys/class/uio/<dev>/maps/map<N>/size before mapping.
     */
    Status connect() override;

    /** Unmap the memory region and close the file descriptor. */
    void disconnect() override;

    [[nodiscard]] bool is_connected() const override;

    /**
     * Read a 32-bit register.
     *
     * @param base_address  Byte offset of the IP base within the UIO map.
     * @param offset        Byte offset of the register within the IP block.
     */
    Result<std::uint32_t> read_register(std::uint32_t base_address,
                                        std::uint32_t offset) override;

    /**
     * Write a 32-bit register.
     *
     * @param base_address  Byte offset of the IP base within the UIO map.
     * @param offset        Byte offset of the register within the IP block.
     * @param value         Value to write.
     */
    Status write_register(std::uint32_t base_address, std::uint32_t offset,
                          std::uint32_t value) override;

    Status execute(hcl_register_operation* operations, std::size_t operation_count) override;

    hcl_register_transport* c_transport{nullptr};
};

}  // namespace hcl
