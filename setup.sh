#!/bin/bash

# get dependencies via wget / apt / or npm

mkdir build
cd build
cmake ..
make -j4
# sudo make install
# sudo ldconfig

exit $?
