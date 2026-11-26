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

#include "hcl/drivers/hololink_i2c.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#if __has_include(<hololink/core/hololink.hpp>)
#include <hololink/core/deserializer.hpp>
#include <hololink/core/hololink.hpp>
#include <hololink/core/serializer.hpp>

namespace hcl::drivers {

struct HololinkI2CContext {
    std::shared_ptr<hololink::Hololink::I2c> i2c;
    bool started{false};
};

static hcl_status i2c_status_error(hcl_status_code code,
                                   std::uint8_t address,
                                   std::uint32_t reg,
                                   std::uint32_t data) {
    const hcl_register_context context = {
        static_cast<std::uint32_t>(address),
        static_cast<std::uint32_t>(reg),
        static_cast<std::uint32_t>(data),
    };
    return hcl_status_error(code, &context);
}

static hcl_status vtable_init(void* context) {
    auto* state = static_cast<HololinkI2CContext*>(context);
    if (state == nullptr || state->i2c == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!state->started) {
        state->started = true;
    }
    return hcl_status_ok();
}

static hcl_status vtable_write(void* context,
                               std::uint8_t address,
                               std::uint8_t reg,
                               std::uint8_t data) {
    auto* state = static_cast<HololinkI2CContext*>(context);
    if (state == nullptr || state->i2c == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(2u);
        hololink::core::Serializer serializer(write_bytes.data(), write_bytes.size());
        if (!serializer.append_uint8(reg) || !serializer.append_uint8(data)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
        }

        const auto read_bytes = state->i2c->i2c_transaction(address, write_bytes, 0u);
        (void)read_bytes;
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    }
}

static hcl_status vtable_read(void* context,
                              std::uint8_t address,
                              std::uint8_t reg,
                              std::uint8_t* out_value) {
    if (out_value == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    auto* state = static_cast<HololinkI2CContext*>(context);
    if (state == nullptr || state->i2c == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(1u);
        hololink::core::Serializer serializer(write_bytes.data(), write_bytes.size());
        if (!serializer.append_uint8(reg)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }

        const auto read_bytes = state->i2c->i2c_transaction(address, write_bytes, 1u);

        hololink::core::Deserializer deserializer(read_bytes.data(), read_bytes.size());
        if (!deserializer.next_uint8(*out_value)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
    }
}

static hcl_status vtable_write_extended(void* context,
                                        std::uint8_t address,
                                        std::uint16_t reg,
                                        std::uint8_t data) {
    auto* state = static_cast<HololinkI2CContext*>(context);
    if (state == nullptr || state->i2c == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(3u);
        hololink::core::Serializer serializer(write_bytes.data(), write_bytes.size());
        if (!serializer.append_uint16_be(reg) || !serializer.append_uint8(data)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
        }

        const auto read_bytes = state->i2c->i2c_transaction(address, write_bytes, 0u);
        (void)read_bytes;
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    }
}

static hcl_status vtable_read_extended(void* context,
                                       std::uint8_t address,
                                       std::uint16_t reg,
                                       std::uint8_t* out_value) {
    if (out_value == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    auto* state = static_cast<HololinkI2CContext*>(context);
    if (state == nullptr || state->i2c == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(2u);
        hololink::core::Serializer serializer(write_bytes.data(), write_bytes.size());
        if (!serializer.append_uint16_be(reg)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }

        const auto read_bytes = state->i2c->i2c_transaction(address, write_bytes, 1u);

        hololink::core::Deserializer deserializer(read_bytes.data(), read_bytes.size());
        if (!deserializer.next_uint8(*out_value)) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
    }
}

static const hcl_i2c_interface vtable = {
    .init = [](hcl_i2c* i2c) {
        auto* state = static_cast<HololinkI2CContext*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        return vtable_init(state);
    },
    .write = [](hcl_i2c* i2c, std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
        auto* state = static_cast<HololinkI2CContext*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        return vtable_write(state, address, reg, data);
    },
    .read = [](hcl_i2c* i2c, std::uint8_t address, std::uint8_t reg, std::uint8_t* out_value) {
        auto* state = static_cast<HololinkI2CContext*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        return vtable_read(state, address, reg, out_value);
    },
    .write_extended = [](hcl_i2c* i2c, std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
        auto* state = static_cast<HololinkI2CContext*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        return vtable_write_extended(state, address, reg, data);
    },
    .read_extended = [](hcl_i2c* i2c, std::uint8_t address, std::uint16_t reg, std::uint8_t* out_value) {
        auto* state = static_cast<HololinkI2CContext*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        return vtable_read_extended(state, address, reg, out_value);
    },
};

}  // namespace

namespace hcl::drivers {

HololinkI2C::HololinkI2C(std::shared_ptr<hololink::Hololink::I2c> i2c)
    : i2c_(std::move(i2c)), driver_(nullptr), started_(false) {
    if (!i2c_) {
        throw std::invalid_argument("hololink i2c instance must not be null");
    }

    auto* context = new HololinkI2CContext{i2c_};
    driver_ = hcl_i2c_create(context, &vtable);
    if (driver_ == nullptr) {
        delete context;
        throw std::runtime_error("failed to create Hololink I2C driver");
    }
}

HololinkI2C::~HololinkI2C() {
    if (driver_ != nullptr) {
        auto* state = static_cast<HololinkI2CContext*>(driver_->context);
        delete state;
        hcl_i2c_destroy(driver_);
        driver_ = nullptr;
    }
}

hcl::Status HololinkI2C::init() {
    if (driver_ == nullptr || i2c_ == nullptr) {
        return hcl::Status::error(StatusCode::InvalidArgument);
    }

    started_ = true;
    return hcl::StatusHelper::to_status(hcl_i2c_init(driver_));
}

hcl::Status HololinkI2C::write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
    return hcl::StatusHelper::to_status(hcl_i2c_write(driver_, address, reg, data));
}

hcl::Result<std::uint8_t> HololinkI2C::read(std::uint8_t address, std::uint8_t reg) {
    std::uint8_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_i2c_read(driver_, address, reg, &value), value);
}

hcl::Status HololinkI2C::write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
    return hcl::StatusHelper::to_status(hcl_i2c_write_extended(driver_, address, reg, data));
}

hcl::Result<std::uint8_t> HololinkI2C::read_extended(std::uint8_t address, std::uint16_t reg) {
    std::uint8_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_i2c_read_extended(driver_, address, reg, &value), value);
}

}  // namespace hcl::drivers

#endif  // __has_include(<hololink/core/hololink.hpp>)
