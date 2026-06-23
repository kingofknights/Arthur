# PowerShell script to download and build Boost inside the Windows container using MinGW

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

# Boost (1.74.0)
Write-Host "Downloading Boost 1.74.0..."
Invoke-WebRequest -UseBasicParsing https://archives.boost.io/release/1.74.0/source/boost_1_74_0.zip -OutFile boost.zip
Write-Host "Extracting Boost (this will take a few minutes)..."
Expand-Archive boost.zip -DestinationPath C:\tmp-build
Set-Location C:\tmp-build\boost_1_74_0
Write-Host "Building Boost b2 engine..."
.\bootstrap.bat gcc
Check-LastExitCode "Failed to bootstrap Boost"
Write-Host "Compiling Boost libraries..."
.\b2 toolset=gcc address-model=64 variant=release link=static threading=multi runtime-link=shared --prefix=C:\local install
Check-LastExitCode "Failed to compile/install Boost"

# Clean up
Set-Location C:\
Remove-Item -Path C:\tmp-build -Recurse -Force
Write-Host "Boost 1.74.0 installed successfully!"
