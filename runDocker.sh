#!/usr/bin/env bash

docker run -ti -p 5900:5900 -v "$(cd "$(dirname "$0")" && pwd)":/workdir zephyr-build:latest
