# Altera Host Control Examples

### C UDP Client Usage

Run the C client with:

```bash
udp_client_example_c --host 127.0.0.1 --port 5005 --count 4 --base 0x00001000 --offset 0x20 --operation read
```

Run a write example with:

```bash
udp_client_example_c --host 127.0.0.1 --port 5005 --count 2 --base 0x00001000 --offset 0x20 --operation write --value 0x12345678
```

Client arguments are: `--host`, `--port`, `--count`, `--base`, `--offset`, `--operation`, `--value`.

### C++ UDP Client Usage

Run the C++ client with:

```bash
udp_client_example --host 127.0.0.1 --port 5005 --count 4 --base 0x00001000 --offset 0x20 --operation read
```

The client sends the desired number of requests and prints responses received from the server.

### Python Client Usage

Run the Python client with:

```bash
python3 python_udp_client_example.py --host 127.0.0.1 --port 5005 --count 4 --base 0x00001000 --offset 0x20 --operation read
```

The Python client supports:

- `--host`
- `--port`
- `--count`
- `--base`
- `--offset`
- `--operation read|write`
- `--value`
- `--timeout`

### Hololink Example Usage

The Hololink example expects a reachable Hololink endpoint and demonstrates creating the Hololink session in the application before passing it to the transport:

```bash
hololink_example --host 127.0.0.1 --base 0x00001000 --offset 0x20 --read
```

Write mode is also available:

```bash
hololink_example --host 127.0.0.1 --base 0x00001000 --offset 0x20 --write --value 0x12345678
```

Arguments are:

- `--host`: Hololink host/IP used for enumeration
- `--base`: register block base address
- `--offset`: register offset
- `--read`: perform a read operation
- `--write`: perform a write operation
- `--value`: value to write when `--write` is selected

### C++ UDP I2C Example Usage

The C++ UDP I2C example demonstrates reading and writing a single register over the UDP transport stack using the Altera I2C driver.

```bash
udp_i2c_example --host 127.0.0.1 --port 4000 --bus 0 --address 0x1A --register 0x20 --read
udp_i2c_example --host 127.0.0.1 --port 4000 --bus 0 --address 0x1A --register16 0x1020 --write --value 0xAB
```

Arguments are:

- `--host`: UDP server host/IP
- `--port`: UDP target port
- `--bus`: I2C bus index on the target device
- `--address`: I2C slave address
- `--register`: 8-bit register index
- `--register16`: 16-bit register index for the extended path
- `--read`: perform a read operation
- `--write`: perform a write operation
- `--value`: value to write when `--write` is selected

### Hololink I2C Example Usage

The C++ Hololink I2C example demonstrates opening a Hololink-backed bus and performing a single register read or write over I2C.

```bash
hololink_i2c_example --host 127.0.0.1 --bus 0 --address 0x1A --register 0x20 --read
hololink_i2c_example --host 127.0.0.1 --bus 0 --address 0x1A --register16 0x1020 --write --value 0xAB
```

Arguments are:

- `--host`: Hololink host/IP used for enumeration
- `--bus`: Hololink I2C bus index
- `--address`: I2C slave address
- `--register`: 8-bit I2C register
- `--register16`: 16-bit I2C register for the extended path
- `--read`: perform a read operation
- `--write`: perform a write operation
- `--value`: value to write when `--write` is selected

Example read output:

```text
Client target: 127.0.0.1:4000 requests=4 base=0x00001000 offset=0x00000020 operation=read
READ request 1: register [base=0x00001000, offset=0x00000020] = 0x12345678
READ request 2: register [base=0x00001000, offset=0x00000020] = 0x12345678
```

### Python UDP I2C Example Usage

This example reads or writes a single I2C register over UDP using the Altera driver backend.

```bash
python3 python_udp_i2c_example.py \
  --driver altera --host 127.0.0.1 --port 4000 --base 0x00000000 \
  --address 0x1A --register 0x20 --operation read

python3 python_udp_i2c_example.py \
  --driver hololink --host 192.168.0.10 --bus 0 \
  --address 0x1A --register 0x20 --operation write --value 0xAB
```

Useful arguments are:

- `--driver`: `altera` or `hololink`
- `--host`, `--port`, `--base`: UDP register transport settings for the Altera backend and the shared target host for Hololink
- `--address`: I2C slave address
- `--register` / `--register16`: 8-bit or 16-bit register index
- `--operation`: `read` or `write`
- `--value`: value to write
- `--extended`: use the 16-bit register path
- `--bus`: I2C bus index to access (Hololink only)

### UDP Server Usage

If you do not have access to hardware, run the example UDP server with:

```bash
udp_server_example --bind 127.0.0.1 --port 4000
```

Server arguments are: `--bind`, `--port`.

The server runs until stopped with `Ctrl+C` at which point request statistics are printed.
