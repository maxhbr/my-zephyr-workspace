#!/usr/bin/env bash

tag="docker.io/zephyrprojectrtos/zephyr-build:latest"

# (
#     cd docker-image/
#     tag=zephyr-build:latest
#     docker build -f Dockerfile.user --build-arg UID=$(id -u) --build-arg GID=$(id -g) -t "$tag"
# )

docker run -ti -p 5900:5900 -v "$(cd "$(dirname "$0")" && pwd)":/workdir "$tag"
