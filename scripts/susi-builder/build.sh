#!/bin/bash

docker build -t susi/builder . || exit $?
docker run -v $(pwd):/output -it susi/builder || exit $?

exit 0
