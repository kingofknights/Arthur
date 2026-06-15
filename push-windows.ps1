# Script to build and push the Windows docker image locally to kingofknights/arthur

$ErrorActionPreference = 'Stop'

$tag = if ($args[0]) { $args[0] } else { "v1.0-msvc" }
$imageName = "kingofknights/arthur"

Write-Host "========================================="
Write-Host "Building Windows Docker image: $imageName:$tag"
Write-Host "========================================="
docker build -f Dockerfile.windows -t "$imageName:$tag" .

Write-Host ""
Write-Host "========================================="
Write-Host "Logging in to Docker Hub"
Write-Host "========================================="
docker login

Write-Host ""
Write-Host "========================================="
Write-Host "Pushing Docker image to Docker Hub"
Write-Host "========================================="
docker push "$imageName:$tag"

Write-Host ""
Write-Host "========================================="
Write-Host "Success! Image pushed to $imageName:$tag"
Write-Host "========================================="
