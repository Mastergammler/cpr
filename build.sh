#!/bin/bash

BUILD_DIR=.build
BIN_PATH="$BUILD_DIR/cpr"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi   

tcc src/compile.c -g -o $BIN_PATH
echo "Main was built!"
