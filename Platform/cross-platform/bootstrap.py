#!/usr/bin/python
import os
import shutil
import platform
import utils
import urllib
import tarfile
import tempfile
import subprocess

# Directories variables
PPAPI_PATH = os.getcwd()
INCLUDE_PATH = os.path.join(PPAPI_PATH, 'include')

ROOT_PATH = os.path.abspath(
  os.path.dirname(os.path.dirname(os.getcwd())));
EPUB3_PATH = os.path.join(ROOT_PATH, 'ePub3')
THIRD_PARTY_PATH = os.path.join(ROOT_PATH, 'ePub3', 'ThirdParty')

# Other variables
SYSTEM = platform.system().lower()

if SYSTEM == "darwin":
    SYSTEM = "mac"

# path => include path
INCLUDE_MAPPING = (
    ('utf8', (
        os.path.join(THIRD_PARTY_PATH, 'utf8-cpp', 'include'),
        os.path.join(THIRD_PARTY_PATH, 'utf8-cpp', 'include', 'utf8')
    )),
    ('google-url', (
        os.path.join(THIRD_PARTY_PATH, 'google-url', 'src'),
        os.path.join(THIRD_PARTY_PATH, 'google-url', 'base')
    )),
    ('libzip', (
        os.path.join(THIRD_PARTY_PATH, 'libzip'),
    )),
    ('ePub3', (
        EPUB3_PATH,
        os.path.join(EPUB3_PATH, 'ePub'),
    )),
    ('ePub3/utilities', (
        os.path.join(EPUB3_PATH, 'utilities'),
    )),
    ('ePub3/xml', (
        os.path.join(EPUB3_PATH, 'xml'),
        os.path.join(EPUB3_PATH, 'xml', 'tree'),
        os.path.join(EPUB3_PATH, 'xml', 'utilities'),
        os.path.join(EPUB3_PATH, 'xml', 'validation')
    ))
)

ALLOWED_HEADER_EXTENSIONS = [".h"]

if SYSTEM == "windows":
    ALLOWED_HEADER_EXTENSIONS += [".inl"]

def copy_header_files(src, dst):
    if not os.path.exists(dst):
        os.mkdir(dst)

    names = [x for x in os.listdir(src)
        if os.path.splitext(x)[1] in ALLOWED_HEADER_EXTENSIONS]

    for name in names:
        shutil.copy(
            os.path.join(src, name),
            dst
        )

# Copy includes in a specific directory
def install_includes():
    print "Create includes"
    # Create include dirname
    if not os.path.exists(INCLUDE_PATH):
        os.mkdir(INCLUDE_PATH)

    for dst, srcPaths in INCLUDE_MAPPING:
        dstPath = os.path.join(INCLUDE_PATH, dst)

        for srcPath in srcPaths:
            copy_header_files(srcPath, dstPath)

# Install gyp
def install_gyp():
    if os.path.exists(os.path.join("vendor", "gyp")):
        return

    print "Clone gyp"
    utils.execute_command(["git", "clone", "https://chromium.googlesource.com/external/gyp.git", "vendor/gyp"])

# Install ninja
def install_ninja():
    if not os.path.exists(os.path.join("vendor", "ninja")):
        print "Clone ninja"
        utils.execute_command(["git", "clone", "https://github.com/ninja-build/ninja.git", "vendor/ninja"])

    # Configure and build ninja
    if SYSTEM == "windows":
        cmd_path = (os.path.join("vendor", "ninja", "ninja.exe"))
    else:
        cmd_path = (os.path.join("vendor", "ninja", "ninja"))

    if not os.path.exists(cmd_path):
        print "Build and install ninja"

        # Initialize visual studio environment variables
        cmd = ["python", "configure.py", "--bootstrap"]

        if SYSTEM == "windows":
            cmd = ["vcvarsall.bat", "&&"] + cmd

        utils.execute_command(cmd, os.path.join("vendor", "ninja"))

# Install libxml2
def install_libxml2():
    libxml2_path = os.path.join("vendor", "libxml2")

    if os.path.exists(libxml2_path):
        return

    print "Download libxml2"
    libxml2_tar_path = os.path.join("vendor", "libxml2.tar.gz")
    urllib.urlretrieve(
            "http://xmlsoft.org/sources/libxml2-2.9.4.tar.gz",
            libxml2_tar_path)

    print "Extract libxml2"
    with tarfile.open(libxml2_tar_path, "r:gz") as libxml2_tar:
        tmp_libxml2_path = tempfile.mkdtemp(prefix="libxml2", dir="vendor")
        libxml2_tar.extractall(tmp_libxml2_path)
        shutil.move(
            os.path.join(tmp_libxml2_path, "libxml2-2.9.4"),
            libxml2_path)
        os.rmdir(tmp_libxml2_path)

    os.remove(libxml2_tar_path)

    # Configure
    if SYSTEM == "windows":
        utils.execute_command(["cscript", "configure.js", "compiler=msvc", "iconv=no"], os.path.join(libxml2_path, "win32"))
    else:
        utils.execute_command(["./configure"], libxml2_path)

# Apply patches
def apply_patches():
    print "Apply patches"
    patch_file_path = os.path.abspath(os.path.join("patches", "%s.diff" % SYSTEM))
    cmd = ["git", "apply"]

    # Add options for windows to manage spaces
    if SYSTEM == "windows":
        cmd += ["--ignore-space-change", "--ignore-whitespace"]

    cmd += [patch_file_path]

    utils.execute_command(cmd, os.path.join("..", ".."))

# Download and install vendors
if not os.path.exists("vendor"):
    os.mkdir("vendor")

install_gyp()
install_ninja()
install_libxml2()

# Apply patches
apply_patches()

# Build includes
install_includes()
