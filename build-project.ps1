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

# Load MSVC environment variables
Import-VCVars

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

# Run CMake configuration targeting C:\local dependencies
# We use Ninja as it is fast and works well with MSVC command prompt environment
cmake -B build -GNinja `
  -DCMAKE_INSTALL_PREFIX="$workspaceDir\dist" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH="C:\local" `
  -DPostgreSQL_ROOT="C:\local" `
  -DBOOST_ROOT="C:\local" `
  $args

# Run build
Write-Host "Compiling project..."
cmake --build build --config Release

# Install build outputs
Write-Host "Installing artifacts to $workspaceDir\dist..."
cmake --install build

# Copy supporting DLLs from C:\local\bin to dist
if (Test-Path "C:\local\bin") {
    Write-Host "Copying supporting DLLs from C:\local\bin to dist..."
    Copy-Item -Path "C:\local\bin\*.dll" -Destination "$workspaceDir\dist" -Force -ErrorAction SilentlyContinue
}

Write-Host "Build completed successfully! Output files are in the 'dist' folder."
