#!/usr/bin/env python3
#
# Copyright (c) 2026 Altera Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Python UDP client example for Altera Host Control bindings."""

from __future__ import annotations

import argparse
import sys

from altera_host_control import RegisterContext, RegisterInterface, UdpRegisterTransport


def parse_u32(value: str) -> int:
    parsed = int(value, 0)
    if parsed < 0 or parsed > 0xFFFFFFFF:
        raise argparse.ArgumentTypeError(f"Value out of range for u32: {value}")
    return parsed


def parse_port(value: str) -> int:
    parsed = int(value, 10)
    if parsed < 1 or parsed > 65535:
        raise argparse.ArgumentTypeError(f"Port out of range: {value}")
    return parsed


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="UDP register client using HCL Python bindings")
    parser.add_argument("--host", default="127.0.0.1", help="Target host (default: 127.0.0.1)")
    parser.add_argument("--port", type=parse_port, default=5005, help="Target UDP port (default: 5005)")
    parser.add_argument("--count", type=int, default=2, help="Number of requests to issue (default: 2)")
    parser.add_argument("--base", type=parse_u32, default=0x00000000, help="Base register address (default: 0x0)")
    parser.add_argument("--offset", type=parse_u32, default=0x00000010, help="Register offset (default: 0x10)")
    parser.add_argument(
        "--operation",
        choices=("read", "write"),
        default="read",
        help="Operation to perform: read or write (default: read)",
    )
    parser.add_argument(
        "--value",
        type=parse_u32,
        default=0x12345678,
        help="Value to write when --operation write is selected (default: 0x12345678)",
    )
    parser.add_argument("--timeout", type=int, default=500, help="Socket timeout in milliseconds (default: 500)")
    return parser


def main() -> int:
    args = build_parser().parse_args()

    if args.count <= 0:
        print(f"Invalid --count value: {args.count}", file=sys.stderr)
        return 2

    transport = UdpRegisterTransport(args.host, args.port, args.timeout)
    access = RegisterInterface(transport)

    connect_status = transport.connect()
    if not connect_status.ok():
        print(f"Connect failed: {connect_status.message}", file=sys.stderr)
        return 1

    print(
        f"Client target: {args.host}:{args.port} requests={args.count} "
        f"base=0x{args.base:08x} offset=0x{args.offset:08x} "
        f"operation={args.operation}"
        f" value=0x{args.value:08x}" if args.operation == "write" else ""
    )

    try:
        for request_index in range(args.count):
            if args.operation == "write":
                write_status = access.write32(args.base, args.offset, args.value)
                if not write_status.ok():
                    print(f"Write failed at request {request_index}: {write_status.message}", file=sys.stderr)
                    return 1
                print(
                    f"WRITE request {request_index + 1}: "
                    f"register [base=0x{args.base:08x}, offset=0x{args.offset:08x}] "
                    f"<- 0x{args.value:08x}"
                )
                continue

            read_result = access.read32(args.base, args.offset)
            if not read_result.ok():
                print(
                    f"Read failed at request {request_index}: {read_result.status.message}",
                    file=sys.stderr,
                )
                return 1

            print(
                f"READ request {request_index + 1}: "
                f"register [base=0x{args.base:08x}, offset=0x{args.offset:08x}] "
                f"= 0x{read_result.value:08x}"
            )
    finally:
        transport.disconnect()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
