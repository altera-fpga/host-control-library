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

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "hcl/drivers/altera_i2c.hpp"
#include "hcl/register_interface.hpp"
#include "hcl/udp_register_transport.hpp"

namespace {

constexpr const char* kDefaultHost = "127.0.0.1";
constexpr std::uint16_t kDefaultPort = 5005u;
constexpr std::uint32_t kDefaultBaseAddress = 0x00000000u;
constexpr std::uint8_t kDefaultAddress = 0x1Au;
constexpr std::uint8_t kDefaultRegister = 0x00u;
constexpr std::uint32_t kDefaultUdpTimeoutMs = 500u;
constexpr std::uint32_t kDefaultSysClkHz = 100'000'000u;
constexpr std::uint32_t kDefaultTransactionTimeoutMs = 1000u;

struct Config {
    std::string host{kDefaultHost};
    std::uint16_t port{kDefaultPort};
    std::uint32_t base_address{kDefaultBaseAddress};
    std::uint8_t address{kDefaultAddress};
    std::uint8_t register8{kDefaultRegister};
    std::optional<std::uint16_t> register16;
    bool write{false};
    std::uint8_t value{0x00u};
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name
              << " [--host HOST] [--port PORT] [--base BASE] [--address ADDRESS]"
              << " [--register REG] [--register16 REG16] [--write | --read] [--value VALUE]\n"
              << "Defaults: --host " << kDefaultHost
              << " --port " << kDefaultPort
              << " --base 0x" << std::hex << kDefaultBaseAddress
              << " --address 0x" << std::uppercase << static_cast<int>(kDefaultAddress)
              << " --register 0x" << static_cast<int>(kDefaultRegister)
              << " --read\n";
    std::cout << std::dec;
}

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

std::uint16_t parse_port(const char* value) {
    char* end = nullptr;
    const long parsed = std::strtol(value, &end, 10);
    if (end == value || *end != '\0' || parsed < 1 || parsed > 65535) {
        std::cerr << "Invalid port: " << value << ". Using default " << kDefaultPort << '\n';
        return kDefaultPort;
    }
    return static_cast<std::uint16_t>(parsed);
}

std::uint32_t parse_u32(const char* value, const char* label, std::uint32_t fallback) {
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(value, &end, 0);
    if (end == value || *end != '\0') {
        std::cerr << "Invalid " << label << ": " << value << ". Using default 0x"
                  << std::hex << fallback << std::dec << '\n';
        return fallback;
    }
    return static_cast<std::uint32_t>(parsed);
}

std::uint8_t parse_u8(const char* value, const char* label, std::uint8_t fallback) {
    const auto parsed = parse_u32(value, label, fallback);
    if (parsed > 0xFFu) {
        std::cerr << "Invalid " << label << ": " << value << ". Using default 0x"
                  << std::hex << static_cast<int>(fallback) << std::dec << '\n';
        return fallback;
    }
    return static_cast<std::uint8_t>(parsed);
}

std::uint16_t parse_u16(const char* value, const char* label, std::uint16_t fallback) {
    const auto parsed = parse_u32(value, label, fallback);
    if (parsed > 0xFFFFu) {
        std::cerr << "Invalid " << label << ": " << value << ". Using default 0x"
                  << std::hex << fallback << std::dec << '\n';
        return fallback;
    }
    return static_cast<std::uint16_t>(parsed);
}

Config parse_args(int argc, char** argv) {
    Config config;

    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(EXIT_SUCCESS);
        }

        if ((arg == "--host" || arg == "--port" || arg == "--base" || arg == "--address" ||
             arg == "--register" || arg == "--register16" || arg == "--value") &&
            index + 1 >= argc) {
            std::cerr << "Missing value for " << arg << '\n';
            print_usage(argv[0]);
            std::exit(EXIT_FAILURE);
        }

        if (arg == "--host") {
            config.host = argv[++index];
        } else if (arg == "--port") {
            config.port = parse_port(argv[++index]);
        } else if (arg == "--base") {
            config.base_address = parse_u32(argv[++index], "base address", kDefaultBaseAddress);
        } else if (arg == "--address") {
            config.address = parse_u8(argv[++index], "address", kDefaultAddress);
        } else if (arg == "--register") {
            config.register8 = parse_u8(argv[++index], "register", kDefaultRegister);
        } else if (arg == "--register16") {
            config.register16 = parse_u16(argv[++index], "register16", 0u);
        } else if (arg == "--write") {
            config.write = true;
        } else if (arg == "--read") {
            config.write = false;
        } else if (arg == "--value") {
            config.value = parse_u8(argv[++index], "value", 0u);
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            print_usage(argv[0]);
            std::exit(EXIT_FAILURE);
        }
    }

    return config;
}

std::string describe_register(const Config& config) {
    if (config.register16.has_value()) {
        return "0x" + [&]() {
            std::ostringstream oss;
            oss << std::hex << std::uppercase << *config.register16;
            return oss.str();
        }();
    }

    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << static_cast<int>(config.register8);
    return oss.str();
}

}  // namespace

int main(int argc, char** argv) {
    const Config config = parse_args(argc, argv);

    auto transport = std::make_shared<hcl::UdpRegisterTransport>(config.host, config.port, kDefaultUdpTimeoutMs);
    const hcl::Status connect_status = transport->connect();
    if (!connect_status.ok()) {
        std::cerr << "Transport connect failed: " << status_name(connect_status.code) << '\n';
        return EXIT_FAILURE;
    }

    auto driver = std::make_unique<hcl::drivers::AlteraI2C>(
        std::make_shared<hcl::RegisterInterface>(transport),
        config.base_address,
        kDefaultSysClkHz,
        std::chrono::milliseconds{kDefaultTransactionTimeoutMs});

    const hcl::Status init_status = driver->init();
    if (!init_status.ok()) {
        std::cerr << "Altera I2C init failed: " << status_name(init_status.code) << '\n';
        transport->disconnect();
        return EXIT_FAILURE;
    }

    if (config.write) {
        if (config.register16.has_value()) {
            const hcl::Status status = driver->write_extended(config.address, *config.register16, config.value);
            if (!status.ok()) {
                std::cerr << "Write failed: " << status_name(status.code) << '\n';
                transport->disconnect();
                return EXIT_FAILURE;
            }
            std::cout << "Wrote 0x" << std::hex << std::uppercase << static_cast<int>(config.value)
                      << " to I2C 0x" << static_cast<int>(config.address)
                      << ", register 0x" << *config.register16 << '\n';
        } else {
            const hcl::Status status = driver->write(config.address, config.register8, config.value);
            if (!status.ok()) {
                std::cerr << "Write failed: " << status_name(status.code) << '\n';
                transport->disconnect();
                return EXIT_FAILURE;
            }
            std::cout << "Wrote 0x" << std::hex << std::uppercase << static_cast<int>(config.value)
                      << " to I2C 0x" << static_cast<int>(config.address)
                      << ", register 0x" << static_cast<int>(config.register8) << '\n';
        }
        transport->disconnect();
        return EXIT_SUCCESS;
    }

    std::uint8_t value = 0u;
    if (config.register16.has_value()) {
        const auto result = driver->read_extended(config.address, *config.register16);
        if (!result.ok()) {
            std::cerr << "Read failed: " << status_name(result.status.code) << '\n';
            transport->disconnect();
            return EXIT_FAILURE;
        }
        value = result.value;
        std::cout << "Read 0x" << std::hex << std::uppercase << static_cast<int>(value)
                  << " from I2C 0x" << static_cast<int>(config.address)
                  << ", register 0x" << *config.register16 << '\n';
    } else {
        const auto result = driver->read(config.address, config.register8);
        if (!result.ok()) {
            std::cerr << "Read failed: " << status_name(result.status.code) << '\n';
            transport->disconnect();
            return EXIT_FAILURE;
        }
        value = result.value;
        std::cout << "Read 0x" << std::hex << std::uppercase << static_cast<int>(value)
                  << " from I2C 0x" << static_cast<int>(config.address)
                  << ", register 0x" << static_cast<int>(config.register8) << '\n';
    }

    transport->disconnect();
    return EXIT_SUCCESS;
}
