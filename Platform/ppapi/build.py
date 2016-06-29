#!/usr/bin/python
import os
import subprocess
import platform

system = platform.system().lower()

if system == "linux":
    # Use clang on linux
    print "Use clang compiler"
    os.environ["CC"] = "clang"
    os.environ["CXX"] = "clang++"
    os.environ["GYP_DEFINES"] = "clang=1"

# Directories variables
PPAPI_PATH = os.getcwd()
GYP_CMD = os.path.join(PPAPI_PATH, "vendor", "gyp", "gyp_main.py")
NINJA_CMD = os.path.join(PPAPI_PATH, "vendor", "ninja", "ninja")

# Create ninja build file
print "Create ninja project"
subprocess.call(["python", GYP_CMD, "--depth=.", "-f", "ninja", "ppapi.gyp"])

# Build ninja project
print "Build ninja project"
subprocess.call([NINJA_CMD, "-C", os.path.join("out", "Default"), "-f", "build.ninja"])