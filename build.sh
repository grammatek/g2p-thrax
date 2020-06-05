#!/bin/bash

mkdir build/ && pushd build/ || exit 1
cmake -DCMAKE_VERBOSE_MAKEFILE=1 .. && make -j