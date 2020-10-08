#!/bin/bash
docker login
docker build \
    -t pythonapi \
    .
docker tag pythonapi $1/pythonapi
docker push $1/pythonapi
