FROM nvidia/cuda:12.6.3-devel-ubuntu24.04

ENV CUDAToolkit_ROOT=/usr/local/cuda

# Python build/runtime dependencies for optional pybind11 bindings.
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        libfmt-dev \
        python3 \
        python3-dev \
        python3-pip \
        python3-venv \
        python-is-python3 \
        pybind11-dev \
        clang-tidy \
        cppcheck \
        flawfinder \
        git \
    && rm -rf /var/lib/apt/lists/*

# Pull in a minimal Hololink SDK used by the optional APB transport.
RUN git clone --depth 1 --branch 2.7.0 https://github.com/nvidia-holoscan/holoscan-sensor-bridge.git /opt/hololink-src \
    && cmake -S /opt/hololink-src -B /opt/hololink-build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/hololink \
        -DHOLOLINK_BUILD_ONLY_NATIVE=ON \
        -DHOLOLINK_BUILD_PYTHON=OFF \
        -DHOLOLINK_BUILD_TESTS=OFF \
        -DHOLOLINK_BUILD_TOOLS=OFF \
        -DHOLOLINK_BUILD_EXAMPLES=OFF \
        -DHOLOLINK_BUILD_EMULATOR=OFF \
        -DHOLOLINK_BUILD_ROCE=OFF \
        -DHOLOLINK_BUILD_CPACK=OFF \
        -DHOLOLINK_BUILD_EXPORT=ON \
    && cmake --build /opt/hololink-build --target install -j"$(nproc)" \
    && rm -rf /opt/hololink-build /opt/hololink-src

ENV CMAKE_PREFIX_PATH=/opt/hololink
ENV Hololink_DIR=/opt/hololink/lib/cmake/Hololink

# Install Statick into an isolated virtual environment and expose the CLI.
RUN python3 -m venv /opt/statick-venv \
    && /opt/statick-venv/bin/pip install --no-cache-dir --upgrade pip \
    && /opt/statick-venv/bin/pip install --no-cache-dir statick \
    && ln -sf /opt/statick-venv/bin/statick /usr/local/bin/statick
