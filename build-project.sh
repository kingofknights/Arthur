#!/bin/bash
# Script to compile a project inside the MinGW-w64 cross-compilation docker container.

set -e

WORKSPACE_DIR="/workspace"

if [ ! -d "$WORKSPACE_DIR" ] || [ -z "$(ls -A "$WORKSPACE_DIR" 2>/dev/null)" ]; then
  echo "Error: Please mount your project source directory to $WORKSPACE_DIR."
  echo "Example: docker run --rm -v \$(pwd):/workspace arthur-mingw-builder"
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
