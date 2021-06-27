#!/bin/bash

mkdir -p build
pushd build
cmake .. -DBOOST_PREFIX=H:/wtm/boost_1_76_0/build
cmake --build . --parallel 9 
popd