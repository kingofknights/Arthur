#!/bin/bash
# Script to build and push the docker image locally to kingofknights/arthur

set -e

# Default to "tagname" if no tag argument is provided
TAG="${1:-tagname}"
IMAGE_NAME="kingofknights/arthur"

echo "========================================="
echo "Building Docker image: $IMAGE_NAME:$TAG"
echo "========================================="
docker build -t "$IMAGE_NAME:$TAG" .

echo ""
echo "========================================="
echo "Logging in to Docker Hub"
echo "========================================="
docker login

echo ""
echo "========================================="
echo "Pushing Docker image to Docker Hub"
echo "========================================="
docker push "$IMAGE_NAME:$TAG"

echo ""
echo "========================================="
echo "Success! Image pushed to $IMAGE_NAME:$TAG"
echo "========================================="
