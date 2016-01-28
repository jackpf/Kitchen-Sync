#!/bin/bash

javac com/jackpf/kitchensync/CInterface/CInterface.java
javah -jni -d ./com/jackpf/kitchensync/CInterface com.jackpf.kitchensync.CInterface.CInterface
