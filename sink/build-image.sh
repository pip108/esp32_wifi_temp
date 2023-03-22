#!/bin/sh
IMAGE='registry.pattern.blue/iot-prometheus'

docker build . -t "${IMAGE}"
docker push "${IMAGE}"