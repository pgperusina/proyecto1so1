#!/bin/bash
docker login
docker build \
    -t lbapi \
    # --build-arg username=$1 \
    # --build-arg password=$2 \
    .
docker tag lbapi $1/lbapi
docker push $1/lbapi
