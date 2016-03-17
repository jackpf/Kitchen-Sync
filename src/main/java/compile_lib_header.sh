#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

javac "$DIR/com/jackpf/kitchensync/CInterface/CInterface.java"
javah -jni -d "$DIR/com/jackpf/kitchensync/CInterface" com.jackpf.kitchensync.CInterface.CInterface
