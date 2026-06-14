# ==========================================
# Stage 1: Build dependencies
# ==========================================
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts during apt installations
ENV DEBIAN_FRONTEND=noninteractive

# Install core build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    curl \
    ca-certificates \
    unzip \
    pkg-config \
    libtool \
    autoconf \
    automake \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    binutils-mingw-w64-x86-64 \
    && rm -rf /var/lib/apt/lists/*

# Copy CMake toolchain file
COPY toolchain-mingw64.cmake /toolchain-mingw64.cmake

# Copy and run dependency build script
COPY build-dependencies.sh /tmp/build-dependencies.sh
RUN chmod +x /tmp/build-dependencies.sh && /tmp/build-dependencies.sh

# ==========================================
# Stage 2: Final development & build image
# ==========================================
FROM ubuntu:22.04

# Prevent interactive prompts during apt installations
ENV DEBIAN_FRONTEND=noninteractive

# Install compiler tools for project compilation
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    binutils-mingw-w64-x86-64 \
    && rm -rf /var/lib/apt/lists/*

# Copy built dependency files from builder stage
COPY --from=builder /usr/x86_64-w64-mingw32 /usr/x86_64-w64-mingw32
COPY --from=builder /toolchain-mingw64.cmake /toolchain-mingw64.cmake

# Copy the build execution script
COPY build-project.sh /usr/local/bin/build-project
RUN chmod +x /usr/local/bin/build-project

# Setup workspace directory
WORKDIR /workspace

# Run build script by default
ENTRYPOINT ["/usr/local/bin/build-project"]
