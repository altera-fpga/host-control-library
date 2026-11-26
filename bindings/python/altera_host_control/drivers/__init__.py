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

try:
    from ..abr_bridge import AbrBridge
except ImportError:
    AbrBridge = None

try:
    from ..altera_i2c import AlteraI2C
except ImportError:
    AlteraI2C = None

try:
    from ..altera_roce import AlteraRoCE
except ImportError:
    AlteraRoCE = None

try:
    from ..hololink_i2c import HololinkI2C
except ImportError:
    HololinkI2C = None

try:
    from ..altera_avalon_pio import AlteraAvalonPIO
except ImportError:
    AlteraAvalonPIO = None

try:
    from ..driveonchip import DriveOnChip
except ImportError:
    DriveOnChip = None

try:
    from ..altera_vvp_blc import AlteraVvpBlc
except ImportError:
    AlteraVvpBlc = None

try:
    from ..altera_vvp_crs import AlteraVvpCrs
except ImportError:
    AlteraVvpCrs = None

try:
    from ..altera_vvp_csc import AlteraVvpCsc
except ImportError:
    AlteraVvpCsc = None

try:
    from ..altera_vvp_demosaic import AlteraVvpDemosaic
except ImportError:
    AlteraVvpDemosaic = None

try:
    from ..altera_vvp_pip_conv import AlteraVvpPipConv
except ImportError:
    AlteraVvpPipConv = None

try:
    from ..altera_vvp_scaler import AlteraVvpScaler
except ImportError:
    AlteraVvpScaler = None

try:
    from ..altera_vvp_switch import AlteraVvpSwitch
except ImportError:
    AlteraVvpSwitch = None

try:
    from ..altera_vvp_tpg import AlteraVvpTpg
except ImportError:
    AlteraVvpTpg = None

try:
    from ..altera_vvp_wbc import AlteraVvpWbc
except ImportError:
    AlteraVvpWbc = None

__all__ = [
    name for name in [
        "AbrBridge",
        "AlteraI2C",
        "AlteraRoCE",
        "AlteraVvpBlc",
        "AlteraVvpCrs",
        "AlteraVvpCsc",
        "AlteraVvpDemosaic",
        "AlteraVvpPipConv",
        "AlteraVvpScaler",
        "AlteraVvpSwitch",
        "AlteraVvpTpg",
        "AlteraVvpWbc",
        "HololinkI2C",
        "AlteraAvalonPIO",
        "DriveOnChip",
    ]
    if globals().get(name) is not None
]
