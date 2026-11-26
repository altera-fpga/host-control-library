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

#include <hololink/core/enumerator.hpp>
#include <hololink/core/hololink.hpp>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

const char* status_name(hcl::StatusCode code) {
    switch (code) {
        case hcl::StatusCode::Ok:
            return "ok";
        case hcl::StatusCode::Timeout:
            return "timeout";
        case hcl::StatusCode::TransportError:
            return "transport_error";
        case hcl::StatusCode::ProtocolError:
            return "protocol_error";
        case hcl::StatusCode::Unsupported:
            return "unsupported";
        case hcl::StatusCode::InvalidArgument:
            return "invalid_argument";
        case hcl::StatusCode::NotConnected:
            return "not_connected";
        case hcl::StatusCode::OutOfRange:
            return "out_of_range";
        case hcl::StatusCode::AllocationFailed:
            return "allocation_failed";
        case hcl::StatusCode::InternalError:
            return "internal_error";
        default:
            return "unknown";
    }
}

struct Options {
    std::string host{"127.0.0.1"};
    std::uint8_t bus{0u};
    std::uint8_t address{0x1Au};
    std::uint16_t register_value{0x20u};
    bool extended{false};
    bool write{false};
    std::uint8_t value{0x00u};
};

std::uint32_t parse_u32(const std::string& text) {
    return static_cast<std::uint32_t>(std::stoul(text, nullptr, 0));
}

std::uint8_t parse_u8(const std::string& text) {
    return static_cast<std::uint8_t>(std::stoul(text, nullptr, 0));
}

Options parse_args(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--host" && index + 1 < argc) {
            options.host = argv[++index];
        } else if (argument == "--bus" && index + 1 < argc) {
            options.bus = static_cast<std::uint8_t>(std::stoul(argv[++index], nullptr, 0));
        } else if (argument == "--address" && index + 1 < argc) {
            options.address = parse_u8(argv[++index]);
        } else if (argument == "--register" && index + 1 < argc) {
            options.register_value = static_cast<std::uint16_t>(std::stoul(argv[++index], nullptr, 0));
            options.extended = false;
        } else if (argument == "--register16" && index + 1 < argc) {
            options.register_value = static_cast<std::uint16_t>(std::stoul(argv[++index], nullptr, 0));
            options.extended = true;
        } else if (argument == "--value" && index + 1 < argc) {
            options.value = parse_u8(argv[++index]);
        } else if (argument == "--read") {
            options.write = false;
        } else if (argument == "--write") {
            options.write = true;
        } else if (argument == "--help" || argument == "-h") {
            std::cout << "Usage: hololink_i2c_example [--host IP] [--bus BUS] [--address HEX] "
                         "[--register HEX | --register16 HEX] [--read|--write [--value HEX]]\n";
            std::exit(0);
        } else {
            throw std::runtime_error("Unknown argument: " + argument);
        }
    }
    return options;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_args(argc, argv);

        const hololink::Metadata metadata = hololink::Enumerator::find_channel(options.host);
        std::shared_ptr<hololink::Hololink> hololink =
            hololink::Hololink::from_enumeration_metadata(metadata);
        if (!hololink) {
            std::cerr << "Failed to create a Hololink session for host '" << options.host << "'\n";
            return 1;
        }

        auto i2c = hololink->get_i2c(options.bus);
        if (!i2c) {
            std::cerr << "Failed to resolve Hololink I2C bus " << static_cast<int>(options.bus) << "\n";
            return 1;
        }

        hcl::drivers::HololinkI2C driver(i2c);
        const hcl::Status init_status = driver.init();
        if (!init_status.ok()) {
            std::cerr << "Failed to initialize Hololink I2C driver: " << status_name(init_status.code) << "\n";
            return 1;
        }

        // Start hololink instance
        hololink->start(); 

        if (options.write) {
            const hcl::Status status = options.extended
                                          ? driver.write_extended(options.address, options.register_value, options.value)
                                          : driver.write(options.address, static_cast<std::uint8_t>(options.register_value), options.value);
            if (!status.ok()) {
                std::cerr << "Write failed: " << status_name(status.code) << "\n";
                hololink->stop(); 
                return 1;
            }
            std::cout << "Wrote 0x" << std::hex << static_cast<int>(options.value) << " to bus "
                      << static_cast<int>(options.bus) << ", address 0x"
                      << static_cast<int>(options.address) << ", reg 0x" << options.register_value << std::dec
                      << "\n";
            hololink->stop();
            return 0;
        }

        const hcl::Result<std::uint8_t> result = options.extended
                                                     ? driver.read_extended(options.address, options.register_value)
                                                     : driver.read(options.address, static_cast<std::uint8_t>(options.register_value));
        if (!result.ok()) {
            std::cerr << "Read failed: " << status_name(result.status.code) << "\n";
            hololink->stop(); 
            return 1;
        }

        std::cout << "Read 0x" << std::hex << static_cast<int>(result.value) << " from bus "
                  << static_cast<int>(options.bus) << ", address 0x"
                  << static_cast<int>(options.address) << ", reg 0x" << options.register_value << std::dec
                  << "\n";
        hololink->stop();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
