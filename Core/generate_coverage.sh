#!/bin/bash

pushd build
rm -rf *
CC=gcc-4.9 CXX=g++-4.9 cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4 susi_test
make -j4 coverage
popd

google-chrome coverage/index.html

exit 0;
