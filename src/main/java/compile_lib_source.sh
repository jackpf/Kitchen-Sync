#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir "$DIR/com/jackpf/kitchensync/CInterface/build"
cd "$DIR/com/jackpf/kitchensync/CInterface/build"
cmake ..
make
make install
#cp ../libkitchensync.dylib /usr/local/lib
