#!/bin/bash
docker login
docker build \
    -t server1 \
    --build-arg username=$1 \
    --build-arg password=$2 \
    .
docker tag server1 $3/server1
docker push $3/server1
