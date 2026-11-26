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

"""
Read or write an I2C register through the Altera UDP register transport.

Examples:
  python3 python_udp_i2c_example.py \
      --host 127.0.0.1 --port 4000 \
      --base 0x00000000 --address 0x1A --register 0x20 --read

  python3 python_udp_i2c_example.py \
      --host 127.0.0.1 --port 4000 \
      --base 0x00000000 --address 0x1A --register 0x20 --write --value 0xAB
"""

from __future__ import annotations

import argparse
import sys
from typing import Any

from altera_host_control import RegisterContext, RegisterInterface, UdpRegisterTransport
from altera_host_control.drivers import AlteraI2C


DEFAULT_UDP_TIMEOUT_MS = 500
DEFAULT_SYS_CLK_HZ = 100_000_000
DEFAULT_TRANSACTION_TIMEOUT_MS = 1000

def parse_u32(value: str) -> int:
    parsed = int(value, 0)
    if parsed < 0 or parsed > 0xFFFFFFFF:
        raise argparse.ArgumentTypeError(f"Value out of range for u32: {value}")
    return parsed


def parse_u16(value: str) -> int:
    parsed = int(value, 0)
    if parsed < 0 or parsed > 0xFFFF:
        raise argparse.ArgumentTypeError(f"Value out of range for u16: {value}")
    return parsed


def parse_u8(value: str) -> int:
    parsed = int(value, 0)
    if parsed < 0 or parsed > 0xFF:
        raise argparse.ArgumentTypeError(f"Value out of range for u8: {value}")
    return parsed


def parse_port(value: str) -> int:
    parsed = int(value, 10)
    if parsed < 1 or parsed > 65535:
        raise argparse.ArgumentTypeError(f"Port out of range: {value}")
    return parsed


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Read or write a single I2C register via the Altera UDP transport."
    )
    parser.add_argument(
        "--host",
        default="127.0.0.1",
        help="UDP host for the Altera backend (default: 127.0.0.1)",
    )
    parser.add_argument(
        "--port",
        type=parse_port,
        default=5005,
        help="UDP port for the Altera backend (default: 5005)",
    )
    parser.add_argument(
        "--base",
        type=parse_u32,
        default=0x00000000,
        help="Base address for the Altera register interface (default: 0x0)",
    )
    parser.add_argument(
        "--address",
        type=parse_u8,
        default=0x1A,
        help="I2C slave address (default: 0x1A)",
    )
    parser.add_argument(
        "--register",
        type=parse_u8,
        default=0x00,
        help="8-bit I2C register to access (default: 0x00)",
    )
    parser.add_argument(
        "--register16",
        type=parse_u16,
        default=None,
        help="16-bit register for the extended access path; if provided, extended mode is enabled automatically",
    )
    parser.add_argument(
        "--operation",
        choices=("read", "write"),
        default="read",
        help="Operation to perform (default: read)",
    )
    parser.add_argument(
        "--value",
        type=parse_u8,
        default=0x00,
        help="Value to write when --operation write is selected (default: 0x00)",
    )
    return parser


def read_register(driver: Any, args: argparse.Namespace) -> int:
    extended = args.register16 is not None
    reg = args.register16 if extended else args.register
    if extended:
        result = driver.read_extended(args.address, reg)
    else:
        result = driver.read(args.address, reg)

    if not result.ok():
        raise RuntimeError(f"Read failed: {result.status.code}")
    return int(result.value)


def write_register(driver: Any, args: argparse.Namespace) -> None:
    extended = args.register16 is not None
    reg = args.register16 if extended else args.register
    if extended:
        status = driver.write_extended(args.address, reg, args.value)
    else:
        status = driver.write(args.address, reg, args.value)

    if not status.ok():
        raise RuntimeError(f"Write failed: {status.code}")


def main() -> int:
    args = build_parser().parse_args()
    transport = None
    driver = None

    try:
        if AlteraI2C is None:
            raise RuntimeError("The Altera I2C backend is not available in this build.")

        transport = UdpRegisterTransport(args.host, args.port, DEFAULT_UDP_TIMEOUT_MS)
        connect_status = transport.connect()
        if not connect_status.ok():
            raise RuntimeError(f"Failed to connect UDP transport: {connect_status.code}")

        register_interface = RegisterInterface(transport)
        driver = AlteraI2C(
            register_interface,
            args.base,
            DEFAULT_SYS_CLK_HZ,
            DEFAULT_TRANSACTION_TIMEOUT_MS,
        )
        init_status = driver.init()
        if not init_status.ok():
            raise RuntimeError(f"Failed to initialize Altera I2C: {init_status.code}")

        if args.operation == "write":
            write_register(driver, args)
            reg = args.register16 if args.register16 is not None else args.register
            print(
                f"Wrote 0x{args.value:02x} to I2C address 0x{args.address:02x}, "
                f"register 0x{reg:04x}"
            )
        else:
            value = read_register(driver, args)
            reg = args.register16 if args.register16 is not None else args.register
            print(
                f"Read from I2C address 0x{args.address:02x}, "
                f"register 0x{reg:04x}: 0x{value:02x}"
            )
        return 0
    except Exception as exc:  # pragma: no cover - CLI feedback path
        print(f"Error: {exc}", file=sys.stderr)
        return 1
    finally:
        if transport is not None:
            transport.disconnect()


if __name__ == "__main__":
    raise SystemExit(main())
