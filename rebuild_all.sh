#!/bin/bash


SOFT=true

if test $# -ge 1 && test $1 = hard; then
	SOFT=false
fi

pushd ./Core/build
test $SOFT = false && rm -rf *
cmake ..
make -j4
popd

pushd ./CPPEngine/build
test $SOFT = false && rm -rf *
cmake ..
make -j4
popd


pushd ./Debugger/build
test $SOFT = false && rm -rf *
cmake ..
make -j4
popd

pushd ./Watchdog/build
test $SOFT = false && rm -rf *
cmake ..
make -j4
popd

exit 0

