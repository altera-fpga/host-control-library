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

#include <array>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace {

constexpr std::uint16_t kDefaultPort = 5005;
constexpr const char* kDefaultBindHost = "0.0.0.0";
constexpr std::uint32_t kReceiveTimeoutMs = 250u;
constexpr std::size_t kMaxDatagramBytes = 4096u;
constexpr std::size_t kMaxOperationsPerMessage = 32u;
constexpr std::uint32_t kProtocolMagic = 0x41434C4Bu;  // "ACLK"
constexpr std::uint8_t kProtocolVersion = 1u;
constexpr std::uint8_t kOpRead = 1u;
constexpr std::uint8_t kOpWrite = 2u;

constexpr std::uint16_t kStatusOk = 0u;
constexpr std::uint16_t kStatusInvalidRequest = 1u;
constexpr std::uint16_t kStatusUnknownOp = 2u;

#pragma pack(push, 1)
struct UdpRequest {
    std::uint32_t magic;
    std::uint8_t version;
    std::uint8_t op;
    std::uint16_t reserved;
    std::uint32_t sequence_id;
    std::uint32_t base_address;
    std::uint32_t offset;
    std::uint32_t value;
};

struct UdpResponse {
    std::uint32_t magic;
    std::uint8_t version;
    std::uint8_t op;
    std::uint16_t status;
    std::uint32_t sequence_id;
    std::uint32_t base_address;
    std::uint32_t offset;
    std::uint32_t value;
};
#pragma pack(pop)

static_assert(sizeof(UdpRequest) == 24, "Unexpected request packet size");
static_assert(sizeof(UdpResponse) == 24, "Unexpected response packet size");

struct ServerConfig {
    std::string bind_host{kDefaultBindHost};
    std::uint16_t port{kDefaultPort};
};

struct ServerStats {
    std::uint32_t handled_requests{0u};
    std::uint32_t read_requests{0u};
    std::uint32_t write_requests{0u};
    std::uint32_t invalid_requests{0u};
};

volatile std::sig_atomic_t g_stop_requested = 0;

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name
              << " [--bind HOST] [--port PORT]\n"
              << "Defaults: --bind " << kDefaultBindHost << " --port " << kDefaultPort << '\n';
}

std::uint16_t parse_port(const char* value) {
    const long parsed = std::strtol(value, nullptr, 10);
    if (parsed <= 0 || parsed > 65535) {
        std::cerr << "Invalid port: " << value << " (using default " << kDefaultPort << ")\n";
        return kDefaultPort;
    }

    return static_cast<std::uint16_t>(parsed);
}

ServerConfig parse_args(int argc, char** argv) {
    ServerConfig config;
    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(EXIT_SUCCESS);
        }
        if ((arg == "--bind" || arg == "--port") && (index + 1) >= argc) {
            std::cerr << "Missing value for " << arg << '\n';
            print_usage(argv[0]);
            std::exit(EXIT_FAILURE);
        }
        if (arg == "--bind") {
            config.bind_host = argv[++index];
            continue;
        }
        if (arg == "--port") {
            config.port = parse_port(argv[++index]);
            continue;
        }

        std::cerr << "Unknown argument: " << arg << '\n';
        print_usage(argv[0]);
        std::exit(EXIT_FAILURE);
    }
    return config;
}

void handle_stop_signal(int) {
    g_stop_requested = 1;
}

void install_signal_handlers() {
    std::signal(SIGINT, handle_stop_signal);
#ifdef SIGTERM
    std::signal(SIGTERM, handle_stop_signal);
#endif
}

void print_stats(const ServerStats& stats) {
    std::cout << "Server summary: handled=" << stats.handled_requests
              << " reads=" << stats.read_requests
              << " writes=" << stats.write_requests
              << " invalid=" << stats.invalid_requests << '\n';
}

#ifdef _WIN32
int close_socket(SOCKET socket_fd) {
    return closesocket(socket_fd);
}
#else
int close_socket(int socket_fd) {
    return close(socket_fd);
}
#endif

}  // namespace

int main(int argc, char** argv) {
    const ServerConfig config = parse_args(argc, argv);

    install_signal_handlers();

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

#ifdef _WIN32
    const SOCKET socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return 1;
    }
#else
    const int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        std::cerr << "socket() failed\n";
        return 1;
    }
#endif

#ifdef _WIN32
    const DWORD timeout_ms = kReceiveTimeoutMs;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout_ms), sizeof(timeout_ms)) != 0) {
        std::cerr << "setsockopt() failed\n";
        close_socket(socket_fd);
        WSACleanup();
        return 1;
    }
#else
    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = static_cast<suseconds_t>(kReceiveTimeoutMs * 1000u);
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
        std::cerr << "setsockopt() failed\n";
        close_socket(socket_fd);
        return 1;
    }
#endif

    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(config.port);
    if (inet_pton(AF_INET, config.bind_host.c_str(), &local_addr.sin_addr) != 1) {
        std::cerr << "Invalid bind host: " << config.bind_host << '\n';
        close_socket(socket_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (bind(socket_fd, reinterpret_cast<const sockaddr*>(&local_addr), sizeof(local_addr)) != 0) {
        std::cerr << "bind() failed\n";
        close_socket(socket_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::unordered_map<std::uint32_t, std::uint32_t> registers;
    ServerStats stats;
    std::cout << "UDP register server listening on " << config.bind_host << ':' << config.port << '\n';
    std::cout << "Press Ctrl+C to stop and print final statistics\n";

    while (g_stop_requested == 0) {
        std::array<std::uint8_t, kMaxDatagramBytes> buffer{};
        sockaddr_in client_addr{};
#ifdef _WIN32
        int client_len = sizeof(client_addr);
        const int recv_len = recvfrom(socket_fd,
                                      reinterpret_cast<char*>(buffer.data()),
                                      static_cast<int>(buffer.size()),
                                      0,
                                      reinterpret_cast<sockaddr*>(&client_addr),
                                      &client_len);
#else
        socklen_t client_len = sizeof(client_addr);
        const int recv_len = recvfrom(socket_fd,
                                      buffer.data(),
                                      buffer.size(),
                                      0,
                                      reinterpret_cast<sockaddr*>(&client_addr),
                                      &client_len);
#endif
        if (recv_len < 0) {
#ifdef _WIN32
            const int socket_error = WSAGetLastError();
            if (g_stop_requested != 0) {
                break;
            }
            if (socket_error == WSAETIMEDOUT || socket_error == WSAEWOULDBLOCK) {
                continue;
            }
#else
            if (g_stop_requested != 0) {
                break;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
            }
#endif
            std::cerr << "recvfrom() failed\n";
            continue;
        }

        std::vector<UdpResponse> responses;

        if (recv_len <= 0 || (recv_len % static_cast<int>(sizeof(UdpRequest))) != 0) {
            UdpResponse response{};
            response.magic = htonl(kProtocolMagic);
            response.version = kProtocolVersion;
            response.op = 0;
            response.status = htons(kStatusInvalidRequest);
            response.sequence_id = 0;
            response.base_address = 0;
            response.offset = 0;
            response.value = 0;
            responses.push_back(response);
            ++stats.invalid_requests;
            ++stats.handled_requests;
        } else {
            const std::size_t request_count = static_cast<std::size_t>(recv_len) / sizeof(UdpRequest);
            const auto* requests = reinterpret_cast<const UdpRequest*>(buffer.data());
            responses.resize(request_count);
            stats.handled_requests += static_cast<std::uint32_t>(request_count);

            if (request_count > kMaxOperationsPerMessage) {
                for (std::size_t req_index = 0; req_index < request_count; ++req_index) {
                    const auto* request = &requests[req_index];
                    auto& response = responses[req_index];
                    response.magic = htonl(kProtocolMagic);
                    response.version = kProtocolVersion;
                    response.op = request->op;
                    response.status = htons(kStatusInvalidRequest);
                    response.sequence_id = request->sequence_id;
                    response.base_address = request->base_address;
                    response.offset = request->offset;
                    response.value = 0;
                }
                ++stats.invalid_requests;
            } else {

                for (std::size_t req_index = 0; req_index < request_count; ++req_index) {
                    const auto* request = &requests[req_index];
                    auto& response = responses[req_index];
                    const std::uint32_t magic = ntohl(request->magic);
                    const std::uint32_t sequence_id = ntohl(request->sequence_id);
                    const std::uint32_t base_address = ntohl(request->base_address);
                    const std::uint32_t offset = ntohl(request->offset);
                    const std::uint32_t address = base_address + offset;
                    const std::uint32_t value = ntohl(request->value);

                    response.magic = htonl(kProtocolMagic);
                    response.version = kProtocolVersion;
                    response.op = request->op;
                    response.status = htons(kStatusOk);
                    response.sequence_id = htonl(sequence_id);
                    response.base_address = htonl(base_address);
                    response.offset = htonl(offset);
                    response.value = 0;

                    if (magic != kProtocolMagic || request->version != kProtocolVersion) {
                        response.status = htons(kStatusInvalidRequest);
                        ++stats.invalid_requests;
                    } else if (request->op == kOpRead) {
                        const auto it = registers.find(address);
                        const std::uint32_t read_value = (it == registers.end()) ? 0u : it->second;
                        response.value = htonl(read_value);
                        ++stats.read_requests;
                        std::cout << "READ  seq=0x" << std::hex << sequence_id
                                  << " base=0x" << base_address
                                  << " offset=0x" << offset
                                  << " addr=0x" << address
                                  << " -> 0x" << read_value << std::dec << '\n';
                    } else if (request->op == kOpWrite) {
                        registers[address] = value;
                        ++stats.write_requests;
                        std::cout << "WRITE seq=0x" << std::hex << sequence_id
                                  << " base=0x" << base_address
                                  << " offset=0x" << offset
                                  << " addr=0x" << address
                                  << " <- 0x" << value << std::dec << '\n';
                    } else {
                        response.status = htons(kStatusUnknownOp);
                        ++stats.invalid_requests;
                    }
                }
            }
        }

#ifdef _WIN32
        const int sent_len = sendto(socket_fd,
                        reinterpret_cast<const char*>(responses.data()),
                        static_cast<int>(responses.size() * sizeof(UdpResponse)),
                                    0,
                                    reinterpret_cast<const sockaddr*>(&client_addr),
                                    client_len);
#else
        const int sent_len = sendto(socket_fd,
                        responses.data(),
                        responses.size() * sizeof(UdpResponse),
                                    0,
                                    reinterpret_cast<const sockaddr*>(&client_addr),
                                    client_len);
#endif
        if (sent_len != static_cast<int>(responses.size() * sizeof(UdpResponse))) {
            std::cerr << "sendto() failed\n";
        }

    }

    std::cout << "Stop requested, shutting down server\n";
    print_stats(stats);

    close_socket(socket_fd);
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
