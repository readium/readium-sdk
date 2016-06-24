#!/bin/bash
CWD=$(pwd)
GYP_PATH="$CWD/vendor/gyp"
NINJA_PATH="$CWD/vendor/ninja"
NACL_SDK_PATH="$CWD/vendor/nacl-sdk"

# GYP: Generate Your Projects
mkdir -p $GYP_PATH
cd $GYP_PATH

if [ ! -e .git ]; then
  # Clone repository
  git clone https://chromium.googlesource.com/external/gyp.git .
fi

# Ninja: Small build system
mkdir -p $NINJA_PATH
cd $NINJA_PATH

if [ ! -e .git ]; then
  # Clone repository
  git clone https://github.com/ninja-build/ninja.git .
  
  # Build binary
  ./bootstrap.py
fi


# NACL SDK
mkdir -p $NACL_SDK_PATH
cd $NACL_SDK_PATH

if [ ! -e naclsdk ]; then
  # Download SDK
  curl -O https://storage.googleapis.com/nativeclient-mirror/nacl/nacl_sdk/nacl_sdk.zip

  # Unzip
  unzip nacl_sdk.zip
  mv nacl_sdk/* .
  
  # Remove archive
  rm -rf nacl_sdk
  rm nacl_sdk.zip
  
  # Install pepper 49
  ./naclsdk install pepper_49
fi
