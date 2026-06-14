#!/bin/bash
# Script to compile a project inside the MinGW-w64 cross-compilation docker container.

set -e

# Determine workspace directory (support default mount /workspace, GitHub Actions /github/workspace, or current directory)
if [ -d "/workspace" ] && [ ! -z "$(ls -A "/workspace" 2>/dev/null)" ]; then
  WORKSPACE_DIR="/workspace"
elif [ -d "/github/workspace" ] && [ ! -z "$(ls -A "/github/workspace" 2>/dev/null)" ]; then
  WORKSPACE_DIR="/github/workspace"
else
  WORKSPACE_DIR="$(pwd)"
fi

if [ -z "$(ls -A "$WORKSPACE_DIR" 2>/dev/null)" ]; then
  echo "Error: Workspace directory $WORKSPACE_DIR is empty."
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
