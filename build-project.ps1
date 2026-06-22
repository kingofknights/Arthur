# PowerShell script to compile Rider project using MSVC and Ninja

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

# Load MSVC environment variables
# Import-VCVars

# Find workspace directory (containing CMakeLists.txt)
$workspaceDir = $null
$searchPaths = @(
    "C:\workspace",
    "C:\github\workspace",
    (Get-Location).Path
)

foreach ($path in $searchPaths) {
    if (Test-Path "$path\CMakeLists.txt") {
        $workspaceDir = $path
        break
    }
}

if ($null -eq $workspaceDir) {
    Write-Error "Could not find CMakeLists.txt in any search path."
    Exit 1
}

Write-Host "Initializing build environment in $workspaceDir..."
Set-Location $workspaceDir

# Configure dependency arguments using vcpkg if available, otherwise fallback to C:\local
$vcpkgToolchain = "C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
$vcpkgArgs = @()

if (Test-Path $vcpkgToolchain) {
    Write-Host "Using vcpkg toolchain for package resolution..."
    $vcpkgArgs = @(
        "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain",
        "-DVCPKG_TARGET_TRIPLET=x64-mingw-static",
        "-DVCPKG_HOST_TRIPLET=x64-mingw-static"
    )
} else {
    Write-Host "vcpkg toolchain not found, falling back to C:\local paths..."
    $vcpkgArgs = @(
        "-DCMAKE_PREFIX_PATH=C:\local",
        "-DPostgreSQL_ROOT=C:\local",
        "-DBOOST_ROOT=C:\local"
    )
}

# Run CMake configuration
cmake -B build -GNinja `
  -DCMAKE_INSTALL_PREFIX="$workspaceDir\dist" `
  -DCMAKE_BUILD_TYPE=Release `
  @vcpkgArgs `
  $args
Check-LastExitCode "CMake configuration failed"

# Run build
Write-Host "Compiling project..."
cmake --build build --config Release
Check-LastExitCode "CMake build failed"

# Install build outputs
Write-Host "Installing artifacts to $workspaceDir\dist..."
cmake --install build
Check-LastExitCode "CMake install failed"

# Copy supporting DLLs from C:\local\bin and vcpkg to dist
if (Test-Path "C:\local\bin") {
    Write-Host "Copying supporting DLLs from C:\local\bin to dist..."
    Copy-Item -Path "C:\local\bin\*.dll" -Destination "$workspaceDir\dist" -Force -ErrorAction SilentlyContinue
}
$vcpkgBin = "C:\vcpkg\installed\x64-mingw-static\bin"
if (Test-Path $vcpkgBin) {
    Write-Host "Copying supporting DLLs from vcpkg to dist..."
    Copy-Item -Path "$vcpkgBin\*.dll" -Destination "$workspaceDir\dist" -Force -ErrorAction SilentlyContinue
}

Write-Host "Build completed successfully! Output files are in the 'dist' folder."
