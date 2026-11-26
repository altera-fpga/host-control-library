# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        pybind11-dev \
        python3 \
        python3-dev \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B /tmp/build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        -DHCL_BUILD_EXAMPLES=ON \
        -DHCL_BUILD_TESTS=OFF \
        -DHCL_BUILD_PYTHON=ON \
        -DHCL_PYTHON_INSTALL_PACKAGE_DIR=/opt/altera/hcl/lib/python/altera_host_control \
    && cmake --build /tmp/build --parallel \
    && cmake --install /tmp/build --prefix /opt/altera/hcl

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libstdc++6 \
        python3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /opt/altera/hcl /opt/altera/hcl

ENV PATH="/opt/altera/hcl/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/altera/hcl/lib:${LD_LIBRARY_PATH}"
ENV PYTHONPATH="/opt/altera/hcl/lib/python:${PYTHONPATH}"

WORKDIR /opt/altera/hcl

CMD ["/bin/bash"]
