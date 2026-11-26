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

from .core import (
    I2C,
    Operation,
    OperationType,
    RegisterContext,
    RegisterInterface,
    Result,
    Status,
    StatusCode,
)

try:
    from .core import HololinkRegisterTransport
except ImportError:
    HololinkRegisterTransport = None

try:
    from .core import UdpRegisterTransport
except ImportError:
    UdpRegisterTransport = None

try:
    from .core import UioRegisterTransport
except ImportError:
    UioRegisterTransport = None

core_exports = [
    "HololinkRegisterTransport",
    "I2C",
    "Operation",
    "OperationType",
    "RegisterContext",
    "RegisterInterface",
    "Result",
    "Status",
    "StatusCode",
    "UioRegisterTransport",
    "UdpRegisterTransport",
]

interface_exports = [
    "I2C",
]

transport_exports = [
    "HololinkRegisterTransport",
    "UioRegisterTransport",
    "UdpRegisterTransport",
]

__all__ = (
    core_exports +
    interface_exports +
    transport_exports
)
