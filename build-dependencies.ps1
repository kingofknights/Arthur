# PowerShell script to download and build all dependencies inside the Windows container using MSVC

$ErrorActionPreference = 'Stop'

function Import-VCVars {
    $vcvarsPaths = @(
        "C:\BuildTools\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    )
    $vcvars = $null
    foreach ($path in $vcvarsPaths) {
        if (Test-Path $path) {
            $vcvars = $path
            break
        }
    }
    if ($vcvars) {
        Write-Host "Found VS Developer Command Prompt: $vcvars"
        $tempFile = [System.IO.Path]::GetTempFileName()
        cmd /c " `"$vcvars`" && set " > $tempFile
        Get-Content $tempFile | ForEach-Object {
            if ($_ -match '^(.*?)=(.*)$') {
                $name = $Matches[1]
                $value = $Matches[2]
                [Environment]::SetEnvironmentVariable($name, $value, [EnvironmentVariableTarget]::Process)
            }
        }
        Remove-Item $tempFile -Force
    } else {
        Write-Warning "Could not find vcvars64.bat in standard paths!"
    }
}

function Check-LastExitCode {
    param([string]$errorMessage)
    if ($LASTEXITCODE -ne 0) {
        Write-Error "$errorMessage (Exit code: $LASTEXITCODE)"
        exit $LASTEXITCODE
    }
}

Import-VCVars

# Create directories
New-Item -ItemType Directory -Path C:\local\include -Force | Out-Null
New-Item -ItemType Directory -Path C:\local\lib -Force | Out-Null
New-Item -ItemType Directory -Path C:\local\bin -Force | Out-Null
New-Item -ItemType Directory -Path C:\tmp-build -Force | Out-Null
Set-Location C:\tmp-build

# 1. PostgreSQL (libpq client library)
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

# 2. fmt (11.1.1)
Write-Host "Building fmt..."
git clone --depth 1 --branch 11.1.1 https://github.com/fmtlib/fmt.git
Check-LastExitCode "Failed to clone fmt"
cmake -B fmt/build -S fmt -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DFMT_TEST=OFF -DCMAKE_BUILD_TYPE=Release
Check-LastExitCode "Failed to configure fmt"
cmake --build fmt/build --target install
Check-LastExitCode "Failed to build/install fmt"

# 3. nlohmann/json (v3.12.0)
Write-Host "Building nlohmann/json..."
git clone --depth 1 --branch v3.12.0 https://github.com/nlohmann/json.git
Check-LastExitCode "Failed to clone json"
cmake -B json/build -S json -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DJSON_BuildTests=OFF -DCMAKE_BUILD_TYPE=Release
Check-LastExitCode "Failed to configure json"
cmake --build json/build --target install
Check-LastExitCode "Failed to build/install json"

# 4. libpqxx (7.10.0)
Write-Host "Building libpqxx..."
git clone --depth 1 --branch 7.10.0 https://github.com/jtv/libpqxx.git
Check-LastExitCode "Failed to clone libpqxx"
cmake -B libpqxx/build -S libpqxx -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DBUILD_TEST=OFF -DCMAKE_BUILD_TYPE=Release -DPostgreSQL_ROOT=C:\local
Check-LastExitCode "Failed to configure libpqxx"
cmake --build libpqxx/build --target install
Check-LastExitCode "Failed to build/install libpqxx"

# 5. SDL (release-3.2.0)
Write-Host "Building SDL3..."
git clone --depth 1 --branch release-3.2.0 https://github.com/libsdl-org/SDL.git
Check-LastExitCode "Failed to clone SDL3"
cmake -B SDL/build -S SDL -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DSDL_STATIC=ON -DSDL_SHARED=OFF -DCMAKE_BUILD_TYPE=Release
Check-LastExitCode "Failed to configure SDL3"
cmake --build SDL/build --target install
Check-LastExitCode "Failed to build/install SDL3"

# 6. SQLiteCpp (3.3.2)
Write-Host "Building SQLiteCpp..."
git clone --depth 1 --branch 3.3.2 https://github.com/SRombauts/SQLiteCpp.git
Check-LastExitCode "Failed to clone SQLiteCpp"
cmake -B SQLiteCpp/build -S SQLiteCpp -GNinja -DCMAKE_INSTALL_PREFIX=C:\local -DSQLITECPP_RUN_CPPLINT=OFF -DSQLITECPP_BUILD_TESTS=OFF -DSQLITECPP_INTERNAL_SQLITE=ON -DCMAKE_BUILD_TYPE=Release
Check-LastExitCode "Failed to configure SQLiteCpp"
cmake --build SQLiteCpp/build --target install
Check-LastExitCode "Failed to build/install SQLiteCpp"

# 7. Boost (1.89.0)
Write-Host "Downloading Boost 1.89.0..."
Invoke-WebRequest -UseBasicParsing https://archives.boost.io/release/1.89.0/source/boost_1_89_0.zip -OutFile boost.zip
Write-Host "Extracting Boost (this will take a few minutes)..."
Expand-Archive boost.zip -DestinationPath C:\tmp-build
Set-Location C:\tmp-build\boost_1_89_0
Write-Host "Building Boost b2 engine..."
.\bootstrap.bat
Check-LastExitCode "Failed to bootstrap Boost"
Write-Host "Compiling Boost libraries..."
.\b2 toolset=msvc address-model=64 variant=release link=static threading=multi runtime-link=shared --prefix=C:\local install
Check-LastExitCode "Failed to compile/install Boost"

# Clean up
Set-Location C:\
Remove-Item -Path C:\tmp-build -Recurse -Force
Write-Host "All dependencies installed successfully!"
