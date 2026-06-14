#!/bin/bash
# Script to build and install dependencies for cross-compiling Arthur/Rider using MinGW-w64 on Linux

set -ex

# Setup build directory
mkdir -p /tmp/build-deps
cd /tmp/build-deps

# 1. PostgreSQL (libpq client library)
echo "Building PostgreSQL libpq client library..."
wget https://ftp.postgresql.org/pub/source/v16.1/postgresql-16.1.tar.gz
tar -xzf postgresql-16.1.tar.gz
cd postgresql-16.1
./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 --without-readline --without-zlib
make -C src/include install
make -C src/interfaces/libpq install
cd ..
rm -rf postgresql-16.1*

# 2. fmt (11.1.1)
echo "Building fmt..."
git clone --depth 1 --branch 11.1.1 https://github.com/fmtlib/fmt.git
cd fmt
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
  -DFMT_TEST=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
cd ..
rm -rf fmt

# 3. nlohmann/json (v3.12.0)
echo "Building nlohmann/json..."
git clone --depth 1 --branch v3.12.0 https://github.com/nlohmann/json.git
cd json
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
  -DJSON_BuildTests=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
cd ..
rm -rf json

# 4. libpqxx (7.10.0)
echo "Building libpqxx..."
git clone --depth 1 --branch 7.10.0 https://github.com/jtv/libpqxx.git
cd libpqxx
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
  -DBUILD_TEST=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
cd ..
rm -rf libpqxx

# 5. SDL (release-3.2.0)
echo "Building SDL3..."
git clone --depth 1 --branch release-3.2.0 https://github.com/libsdl-org/SDL.git
cd SDL
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
  -DSDL_TESTS=OFF \
  -DSDL_EXAMPLES=OFF \
  -DSDL_STATIC=ON \
  -DSDL_SHARED=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
cd ..
rm -rf SDL

# 6. SQLiteCpp (3.3.2)
echo "Building SQLiteCpp..."
git clone --depth 1 --branch 3.3.2 https://github.com/SRombauts/SQLiteCpp.git
cd SQLiteCpp
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
  -DSQLITECPP_RUN_CPPLINT=OFF \
  -DSQLITECPP_BUILD_TESTS=OFF \
  -DSQLITECPP_INTERNAL_SQLITE=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install
cd ..
rm -rf SQLiteCpp

# 7. Boost (1.74.0)
echo "Building Boost 1.74.0..."
wget https://boostorg.jfrog.io/artifactory/main/release/1.74.0/source/boost_1_74_0.tar.gz
tar -xzf boost_1_74_0.tar.gz
cd boost_1_74_0
./bootstrap.sh --prefix=/usr/x86_64-w64-mingw32
echo "using gcc : mingw32 : x86_64-w64-mingw32-g++ ;" > user-config.jam
# Build and install libraries (e.g. system, thread, filesystem, etc.)
./b2 --user-config=user-config.jam \
     toolset=gcc-mingw32 \
     target-os=windows \
     address-model=64 \
     link=static \
     threading=multi \
     runtime-link=shared \
     --prefix=/usr/x86_64-w64-mingw32 \
     -j$(nproc) \
     install
cd ..
rm -rf boost_1_74_0*

# Clean up
cd /
rm -rf /tmp/build-deps
echo "All dependencies compiled and installed successfully!"
