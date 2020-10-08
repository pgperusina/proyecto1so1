#!/bin/bash
docker login
docker build \
    -t lbapi \
    .
docker tag lbapi $1/lbapi
docker push $1/lbapi
