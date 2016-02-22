#!/bin/bash

mkdir ./com/jackpf/kitchensync/CInterface/build
cd ./com/jackpf/kitchensync/CInterface/build
cmake ..
make
make install
#cp ../libkitchensync.dylib /usr/local/lib