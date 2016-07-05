#!/usr/bin/python
import os
import subprocess
import platform
import utils

system = platform.system().lower()

if system == "linux":
    # Use clang on linux
    # print "Use clang compiler"
    # os.environ["CC"] = "clang"
    # os.environ["CXX"] = "clang++"
    # os.environ["GYP_DEFINES"] = "clang=1"
elif system == "windows":
    # Use msvs version 2015
    print "Use msvs version 2015"
    os.environ["GYP_MSVS_VERSION"] = "2015"

# Directories variables
PPAPI_PATH = os.getcwd()
GYP_CMD = os.path.join(PPAPI_PATH, "vendor", "gyp", "gyp_main.py")
NINJA_CMD = os.path.join(PPAPI_PATH, "vendor", "ninja", "ninja")

# Create ninja build file
print "Create ninja project"
utils.execute_command(["python", GYP_CMD, "--depth=.", "-f", "ninja", "ppapi.gyp"])

# Build ninja project
print "Build ninja project"
utils.execute_command([NINJA_CMD, "-C", os.path.join("out", "Default"), "-f", "build.ninja"])