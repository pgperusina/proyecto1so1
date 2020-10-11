#!/bin/bash
docker login
docker build \
    -t server2 \
    .
docker tag server2 $1/server2
docker push $1/server2
