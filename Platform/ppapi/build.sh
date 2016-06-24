#!/bin/bash
CWD=$(pwd)
GYP_CMD="$CWD/vendor/gyp/gyp_main.py"
NINJA_CMD="$CWD/vendor/ninja/ninja"


export CC=clang
export CXX=clang++
export GYP_DEFINES="clang=1"

# Create ninja build file
$GYP_CMD --depth=. -f ninja ppapi.gyp



$NINJA_CMD -C out/Default/ -f build.ninja