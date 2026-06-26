# PowerShell script to download and build specific dependencies inside the Windows container using MinGW

$ErrorActionPreference = 'Stop'

function Check-LastExitCode {
    param([string]$errorMessage)
    if ($LASTEXITCODE -ne 0) {
        Write-Error "$errorMessage (Exit code: $LASTEXITCODE)"
        exit $LASTEXITCODE
    }
}

# Create directories
New-Item -ItemType Directory -Path C:\local\include -Force | Out-Null
New-Item -ItemType Directory -Path C:\local\lib -Force | Out-Null
New-Item -ItemType Directory -Path C:\local\bin -Force | Out-Null
New-Item -ItemType Directory -Path C:\tmp-build -Force | Out-Null
Set-Location C:\tmp-build

# 1. PostgreSQL (libpq client library)
# We download and extract pre-compiled client binaries to C:\local.
# This prevents duplicate symbol definitions with winpthreads when libpq is compiled statically via vcpkg.
Write-Host "Downloading PostgreSQL binaries..."
Invoke-WebRequest -UseBasicParsing https://sbp.enterprisedb.com/get/db/postgresql-16.1-1-windows-x64-binaries.zip -OutFile postgresql.zip
Write-Host "Extracting PostgreSQL..."
Expand-Archive postgresql.zip -DestinationPath C:\tmp-build
# Copy headers and libraries
Copy-Item -Path C:\tmp-build\pgsql\include\* -Destination C:\local\include -Recurse -Force
Copy-Item -Path C:\tmp-build\pgsql\lib\libpq.lib -Destination C:\local\lib -Force
Copy-Item -Path C:\tmp-build\pgsql\bin\libpq.dll -Destination C:\local\bin -Force
Copy-Item -Path C:\tmp-build\pgsql\bin\libcrypto*.dll -Destination C:\local\bin -Force
Copy-Item -Path C:\tmp-build\pgsql\bin\libssl*.dll -Destination C:\local\bin -Force

# 2. libpqxx (7.10.0)
# Build libpqxx statically using MinGW, linking against the dynamic libpq from C:\local
Write-Host "Building libpqxx..."
git clone --depth 1 --branch 7.10.0 https://github.com/jtv/libpqxx.git
Check-LastExitCode "Failed to clone libpqxx"
cmake -B libpqxx/build -S libpqxx -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DBUILD_TEST=OFF -DCMAKE_BUILD_TYPE=Release -DPostgreSQL_ROOT=C:\local
Check-LastExitCode "Failed to configure libpqxx"
cmake --build libpqxx/build --target install
Check-LastExitCode "Failed to build/install libpqxx"

# 3. Boost (1.74.0)
Write-Host "Downloading Boost 1.74.0..."
Invoke-WebRequest -UseBasicParsing https://archives.boost.io/release/1.74.0/source/boost_1_74_0.zip -OutFile boost.zip
Write-Host "Extracting Boost (this will take a few minutes)..."
Expand-Archive boost.zip -DestinationPath C:\tmp-build
Set-Location C:\tmp-build\boost_1_74_0
Write-Host "Building Boost b2 engine..."
.\bootstrap.bat gcc
Check-LastExitCode "Failed to bootstrap Boost"
Write-Host "Compiling Boost libraries..."
.\b2 toolset=gcc address-model=64 variant=release link=static threading=multi runtime-link=shared --prefix=C:\local --layout=system install
Check-LastExitCode "Failed to compile/install Boost"

# Clean up
Set-Location C:\
Remove-Item -Path C:\tmp-build -Recurse -Force
Write-Host "Boost 1.74.0 and libpqxx installed successfully!"
