# mingw-toolchain.cmake
# Toolchain file for cross-compiling to Windows (x86_64) using MinGW-w64 on Linux

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Cross compilers to use for C, C++ and Resource Files
# We use the -posix variants on Debian/Ubuntu to enable C++11 threading support (std::thread, std::mutex, etc.)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Root directory of the target environment
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32 /opt/mingw)
set(CMAKE_PREFIX_PATH /opt/mingw)

# Adjust the default behavior of the FIND_XXX() commands:
# Search for programs in the host environment, but search for libraries and headers in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Ensure CMake knows where to find libraries installed under /opt/mingw
link_directories(/opt/mingw/lib)
include_directories(/opt/mingw/include)
