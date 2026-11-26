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

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "hcl/register_interface.hpp"
#include "hcl/uio_register_transport.hpp"

namespace {

constexpr const char* kDefaultDevicePath = "/dev/uio0";
constexpr unsigned int kDefaultMapIndex = 0u;
constexpr std::uint32_t kDefaultBaseAddress = 0x00000000u;
constexpr std::uint32_t kDefaultOffset = 0x00000010u;
constexpr std::uint32_t kInitialWriteValue = 0x12345678u;

enum class OperationMode {
    kRead,
    kWrite,
};

struct ClientConfig {
    std::string device_path{kDefaultDevicePath};
    unsigned int map_index{kDefaultMapIndex};
    std::uint32_t base_address{kDefaultBaseAddress};
    std::uint32_t offset{kDefaultOffset};
    OperationMode operation{OperationMode::kRead};
    std::uint32_t write_value{kInitialWriteValue};
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name
              << " [--device PATH] [--map-index INDEX]"
              << " [--base BASE_ADDRESS] [--offset OFFSET]"
              << " [--operation read|write] [--value WRITE_VALUE]\n"
              << "Defaults: --device " << kDefaultDevicePath
              << " --map-index " << kDefaultMapIndex
              << " --base 0x" << std::hex << kDefaultBaseAddress
              << " --offset 0x" << kDefaultOffset
              << " --operation read"
              << " --value 0x" << kInitialWriteValue << std::dec << '\n';
}

const char* operation_name(OperationMode operation) {
    return operation == OperationMode::kRead ? "read" : "write";
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

unsigned int parse_map_index(const char* value) {
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed > 0xFFFFu) {
        std::cerr << "Invalid map index: " << value << " (using default " << kDefaultMapIndex << ")\n";
        return kDefaultMapIndex;
    }
    return static_cast<unsigned int>(parsed);
}

std::uint32_t parse_u32_value(const char* value, const char* label, std::uint32_t fallback) {
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(value, &end, 0);
    if (end == value || *end != '\0') {
        std::cerr << "Invalid " << label << ": " << value << " (using default 0x"
                  << std::hex << fallback << std::dec << ")\n";
        return fallback;
    }
    return static_cast<std::uint32_t>(parsed);
}

OperationMode parse_operation(const char* value) {
    const std::string operation = value;
    if (operation == "read") {
        return OperationMode::kRead;
    }
    if (operation == "write") {
        return OperationMode::kWrite;
    }

    std::cerr << "Invalid operation: " << value << " (using default read)\n";
    return OperationMode::kRead;
}

ClientConfig parse_args(int argc, char** argv) {
    ClientConfig config;
    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(EXIT_SUCCESS);
        }

        if ((arg == "--device" || arg == "--map-index" || arg == "--base" || arg == "--offset" ||
             arg == "--operation" || arg == "--value") &&
            (index + 1) >= argc) {
            std::cerr << "Missing value for " << arg << '\n';
            print_usage(argv[0]);
            std::exit(EXIT_FAILURE);
        }

        if (arg == "--device") {
            config.device_path = argv[++index];
            continue;
        }
        if (arg == "--map-index") {
            config.map_index = parse_map_index(argv[++index]);
            continue;
        }
        if (arg == "--base") {
            config.base_address = parse_u32_value(argv[++index], "base address", kDefaultBaseAddress);
            continue;
        }
        if (arg == "--offset") {
            config.offset = parse_u32_value(argv[++index], "offset", kDefaultOffset);
            continue;
        }
        if (arg == "--operation") {
            config.operation = parse_operation(argv[++index]);
            continue;
        }
        if (arg == "--value") {
            config.write_value = parse_u32_value(argv[++index], "write value", kInitialWriteValue);
            continue;
        }

        std::cerr << "Unknown argument: " << arg << '\n';
        print_usage(argv[0]);
        std::exit(EXIT_FAILURE);
    }

    return config;
}

}  // namespace

int main(int argc, char** argv) {
    const ClientConfig config = parse_args(argc, argv);

    auto transport = std::make_shared<hcl::UioRegisterTransport>(config.device_path, config.map_index);
    const hcl::Status connect_status = transport->connect();
    if (!connect_status.ok()) {
        std::cerr << "Transport connect failed: " << status_name(connect_status.code) << '\n';
        return EXIT_FAILURE;
    }

    hcl::RegisterInterface access(transport);

    std::cout << "UIO target: " << config.device_path
              << " map_index=" << config.map_index
              << " base=0x" << std::hex << config.base_address
              << " offset=0x" << config.offset
              << " operation=" << operation_name(config.operation);
    if (config.operation == OperationMode::kWrite) {
        std::cout << " value=0x" << config.write_value;
    }
    std::cout << std::dec << '\n';

    if (config.operation == OperationMode::kWrite) {
        const auto write_status = access.write32(config.base_address, config.offset, config.write_value);
        if (!write_status.ok()) {
            std::cerr << "Write failed: " << status_name(write_status.code) << '\n';
            transport->disconnect();
            return EXIT_FAILURE;
        }

        std::cout << "WRITE: register [base=0x" << std::hex << config.base_address
                  << ", offset=0x" << config.offset << "] <- 0x" << config.write_value << std::dec << '\n';
        transport->disconnect();
        return EXIT_SUCCESS;
    }

    const auto read_result = access.read32(config.base_address, config.offset);
    if (!read_result.ok()) {
        std::cerr << "Read failed: " << status_name(read_result.status.code) << '\n';
        transport->disconnect();
        return EXIT_FAILURE;
    }

    std::cout << "READ: register [base=0x" << std::hex << config.base_address
              << ", offset=0x" << config.offset << "] = 0x" << read_result.value << std::dec << '\n';

    transport->disconnect();
    return EXIT_SUCCESS;
}
