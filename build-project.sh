#!/bin/bash
# Script to compile a project inside the MinGW-w64 cross-compilation docker container.

set -e

# Determine workspace directory (search for CMakeLists.txt to verify valid project directory)
if [ -f "/workspace/CMakeLists.txt" ]; then
  WORKSPACE_DIR="/workspace"
elif [ -f "/github/workspace/CMakeLists.txt" ]; then
  WORKSPACE_DIR="/github/workspace"
elif [ -f "$(pwd)/CMakeLists.txt" ]; then
  WORKSPACE_DIR="$(pwd)"
elif [ -d "/workspace" ] && [ ! -z "$(ls -A "/workspace" 2>/dev/null)" ]; then
  WORKSPACE_DIR="/workspace"
elif [ -d "/github/workspace" ] && [ ! -z "$(ls -A "/github/workspace" 2>/dev/null)" ]; then
  WORKSPACE_DIR="/github/workspace"
else
  WORKSPACE_DIR="$(pwd)"
fi

if [ ! -f "$WORKSPACE_DIR/CMakeLists.txt" ]; then
  echo "Error: Could not find CMakeLists.txt in $WORKSPACE_DIR."
  exit 1
fi

cd "$WORKSPACE_DIR"

echo "Initializing build environment in $WORKSPACE_DIR..."

# Run CMake configuration
cmake -B build -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=/toolchain-mingw64.cmake \
  -DCMAKE_INSTALL_PREFIX="$WORKSPACE_DIR/dist" \
  -DCMAKE_BUILD_TYPE=Release \
  "$@"

# Run build
echo "Compiling project..."
cmake --build build --config Release

# Install build outputs (executables, DLLs, etc.)
echo "Installing artifacts to $WORKSPACE_DIR/dist..."
cmake --install build

echo "Build completed successfully! Output files are in the 'dist' folder."
