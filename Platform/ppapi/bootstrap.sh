#!/bin/bash
CWD=$(pwd)
VENDOR_GYP_PATH="$CWD/vendor/gyp"
VENDOR_NINJA_PATH="$CWD/vendor/ninja"

# GYP: Generate Your Projects
mkdir -p $VENDOR_GYP_PATH
cd $VENDOR_GYP_PATH

if [ -e .git ]; then
  # Repository already exists. Update
  git pull
else
  # Clone repository
  git clone https://chromium.googlesource.com/external/gyp.git .
fi

# Ninja: Small build system
mkdir -p $VENDOR_NINJA_PATH
cd $VENDOR_NINJA_PATH

if [ -e .git ]; then
  # Repository already exists. Update
  git pull
else
  # Clone repository
  git clone https://github.com/ninja-build/ninja.git .
fi

./bootstrap.py