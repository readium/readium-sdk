#!/usr/bin/python
import os
import shutil

# Directories variables
PPAPI_PATH = os.getcwd()
INCLUDE_PATH = os.path.join(PPAPI_PATH, 'include')

ROOT_PATH = os.path.abspath(
  os.path.dirname(os.path.dirname(os.getcwd())));
EPUB3_PATH = os.path.join(ROOT_PATH, 'ePub3')
THIRD_PARTY_PATH = os.path.join(ROOT_PATH, 'ePub3', 'ThirdParty')  

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


def copy_header_files(src, dst):
    if not os.path.exists(dst):
        os.mkdir(dst)
        
    names = [x for x in os.listdir(src) if x.endswith('.h')]
      
    for name in names:
        shutil.copy(
            os.path.join(src, name), 
            dst
        )

# Copy includes in a specific directory
def build_includes():
    # Create include dirname
    if not os.path.exists(INCLUDE_PATH):
        os.mkdir(INCLUDE_PATH)

    for dst, srcPaths in INCLUDE_MAPPING:
        dstPath = os.path.join(INCLUDE_PATH, dst)

        for srcPath in srcPaths:
            copy_header_files(srcPath, dstPath)


# Build includes
build_includes()