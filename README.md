# Altera Host Control Library

`altera-host-control` provides C and C++ APIs for reading and writing memory-mapped registers over pluggable transports.

The core library includes a transport abstraction, a register access layer, UDP and UIO transports, optional Python bindings, and optional higher-level drivers and sensor helpers built on top of the register interface.

## Components

### Core library

- C transport abstraction via `hcl_register_transport`
- C register access layer via `hcl_register_interface`
- C++ transport abstraction via `hcl::RegisterTransport`
- C++ register helper via `hcl::RegisterInterface`

### Transports

- UDP register transport
- UIO register transport
- Hololink APB register transport

## Build

Default build:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Docker builds:

```bash
docker build -t altera-host-control .
docker build -f Dockerfile.hololink -t altera-host-control:hololink .
```

Use `Dockerfile.hololink` when you need the optional NVIDIA Hololink transport, drivers and examples.

Useful CMake options:

- `HCL_BUILD_CPP=ON|OFF`: build C++ wrappers, C++ examples, and C++ tests
- `HCL_BUILD_EXAMPLES=ON|OFF`: build example programs
- `HCL_BUILD_TESTS=ON|OFF`: build tests
- `HCL_BUILD_DRIVERS=ON|OFF`: build optional driver libraries
- `HCL_BUILD_SENSORS=ON|OFF`: build optional sensor helpers when drivers are enabled
- `HCL_BUILD_PYTHON=ON|OFF`: build Python bindings when dependencies are available
- `HCL_BUILD_HOLOLINK=ON|OFF`: build the optional Hololink-backed register transport; requires a Hololink SDK package exposing `hololink::core`

Example: build core library, drivers, and sensors without tests:

```bash
cmake -S . -B build -DACL_BUILD_TESTS=OFF -DACL_BUILD_DRIVERS=ON -DACL_BUILD_SENSORS=ON
cmake --build build
```

### Python bindings

Enable bindings with:

```bash
cmake -S . -B build -DACL_BUILD_PYTHON=ON -DACL_BUILD_DRIVERS=ON -DACL_BUILD_SENSORS=ON
cmake --build build
```

Bindings require:

- Python 3 interpreter and development headers
- `pybind11`

The Python package is split into a small core API and optional grouped namespaces:

- `altera_host_control`: core interfaces, result/status types, and transports
- `altera_host_control.drivers`: hardware drivers
- `altera_host_control.sensors`: sensor drivers


By default, install places the package in the selected interpreter's site-packages directory when available. You can override the install destination with `-DACL_PYTHON_INSTALL_PACKAGE_DIR=/path/to/site-packages/altera_host_control`.

## UDP Protocol (v1)

### Request packet (24 bytes)

- `magic` (`u32`): `0x41434C4B` (`"HCLK"`)
- `version` (`u8`): `1`
- `op` (`u8`): `1=read`, `2=write`
- `reserved` (`u16`): `0`
- `sequence_id` (`u32`): request ID echoed by the response
- `base_address` (`u32`): register block base address in network byte order
- `offset` (`u32`): register offset from base address in network byte order
- `value` (`u32`): write value for `op=2`, ignored for reads

### Response packet (24 bytes)

- `magic` (`u32`): `0x41434C4B`
- `version` (`u8`): `1`
- `op` (`u8`): mirrors request op
- `status` (`u16`): `0=ok`, non-zero indicates device-side failure
- `sequence_id` (`u32`): same as request correlation ID
- `base_address` (`u32`): same as request base address in network byte order
- `offset` (`u32`): same as request offset in network byte order
- `value` (`u32`): read value for `op=1`, ignored for writes

## Examples

See `examples/README.md` for runnable example commands.
