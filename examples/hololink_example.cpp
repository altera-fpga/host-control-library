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

#include "hcl/hololink_register_transport.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <hololink/core/enumerator.hpp>
#include <hololink/core/hololink.hpp>

namespace {

struct Options {
    std::string host{"127.0.0.1"};
    std::uint32_t base_address{0x00001000u};
    std::uint32_t offset{0x00000000u};
    bool write{false};
    std::uint32_t value{0x12345678u};
};

std::uint32_t parse_u32(const char* text) {
    return static_cast<std::uint32_t>(std::stoul(text, nullptr, 0));
}

Options parse_args(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--host" && index + 1 < argc) {
            options.host = argv[++index];
        } else if (argument == "--base" && index + 1 < argc) {
            options.base_address = parse_u32(argv[++index]);
        } else if (argument == "--offset" && index + 1 < argc) {
            options.offset = parse_u32(argv[++index]);
        } else if (argument == "--value" && index + 1 < argc) {
            options.value = parse_u32(argv[++index]);
            options.write = true;
        } else if (argument == "--read") {
            options.write = false;
        } else if (argument == "--write") {
            options.write = true;
        } else if (argument == "--help" || argument == "-h") {
            std::cout
                << "Usage: hololink_example [--host IP] [--base HEX] [--offset HEX] [--read|--write --value HEX]\n";
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
            std::cerr << "Failed to create Hololink instance\n";
            return 1;
        }

        hcl::HololinkRegisterTransport transport(hololink);
        const hcl::Status connect_status = transport.connect();
        if (!connect_status.ok()) {
            std::cerr << "Transport connect failed\n";
            return 1;
        }

        if (options.write) {
            const hcl::Status write_status = transport.write_register(options.base_address,
                                                                      options.offset,
                                                                      options.value);
            if (!write_status.ok()) {
                std::cerr << "Write failed\n";
                transport.disconnect();
                return 1;
            }

            std::cout << "Wrote 0x" << std::hex << options.value << " to base=0x" << options.base_address
                      << " offset=0x" << options.offset << std::dec << "\n";
        } else {
            const hcl::Result<std::uint32_t> value = transport.read_register(options.base_address, options.offset);
            if (!value.ok()) {
                std::cerr << "Read failed\n";
                transport.disconnect();
                return 1;
            }

            std::cout << "Read 0x" << std::hex << value.value << " from base=0x" << options.base_address
                      << " offset=0x" << options.offset << std::dec << "\n";
        }

        transport.disconnect();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
