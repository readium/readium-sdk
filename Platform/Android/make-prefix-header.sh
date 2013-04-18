#!/bin/bash
#
#  make-prefix-header.sh
#  ePub3
#
#  Created by Jim Dovey on 2013-04-18.
#  Copyright (c) 2012-2013 The Readium Foundation and contributors.
#  
#  The Readium SDK is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

if [ "$2" = "" ]; then
	echo "Usage: make-prefix-header.sh <arm|mips|x86> <ndk-toolchain-version> [path-to-ndk]"
    echo "Examples"
    echo "  GCC 4.7 targetting MIPS:"
    echo "        make-prefix-header.sh mips 4.7 <ndk-path>"
    echo "  LLVM Clang 3.2 targetting ARM:"
    echo "        make-prefix-header.sh arm clang3.2 <ndk-path>"
	exit 1
fi

host_arch=`uname -m`
case "$host_arch" in
    i?86)
        host_arch=x86
        ;;
    amd64|x86_64)
        host_arch=x86_64
        ;;
esac

host_os=`uname -s`
case "$host_os" in
    Darwin)
        host_os=darwin
        ;;
    Linux)
        host_os=linux
        ;;
    FreeBsd)
        host_os=freebsd
        ;;
    CYGWIN*|*_NT-*)
        host_os=windows
        if [ "x$OSTYPE" = xcygwin ] ; then
            host_os=cygwin
        fi
        ;;
esac

ndk_root=$3
if [ -z "$ndk_root" ]; then
    ndk_root=`which ndk-build`
    if [ -z "$ndk_root" ]; then
        echo "ERROR: You need to provide an <ndk-root>!"
        exit 1
    fi
    $ndk_root=`dirname $ndk_root`
    echo "Using ndk_root = $ndk_root"
fi

platform=""
case "$host_os" in
    linux)
        platform=linux-$host_arch
        ;;
    darwin|freebsd)
        platform=darwin-$host_arch
        ;;
    windows|cygwin)
        platform=windows-$host_arch
        ;;
    *)
        platform=linux-$host_arch
        ;;
esac

target_base=""
case "$1" in
    arm)
        target_base=arm-linux-androideabi
        ;;
    mips)
        target_base=mipsel-linux-android
        ;;
    x86|i?86)
        target_base=x86
        ;;
    *)
        echo "Unknown target architecture: $2"
        exit 1
        ;;
esac

target=$target_base-$2

toolchain=$target
ccompiler=$target-gcc
cxxcompiler=$target-g++
gnuvers=$2
gcc_toolchain=""
case "$2" in
    clang3.1)
        toolchain="llvm-3.1"
        ccompiler=clang
        cxxcompiler=clang++
        gnuvers="4.7"
        gcc_toolchain="-gcc-toolchain $ndk_root/toolchains/$target_base-$gnuvers/prebuilt/$platform"
        ;;
    clang3.2)
        toolchain="llvm-3.2"
        ccompiler=clang
        cxxcompiler=clang++
        gnuvers="4.7"
        gcc_toolchain="-gcc-toolchain $ndk_root/toolchains/$target_base-$gnuvers/prebuilt/$platform"
        ;;
esac

ndk_release_file=$ndk_root"/RELEASE.TXT"
ndk_releaseno=`cat $ndk_release_file | sed 's/^r\(.*\)$/\1/g'`

echo "Detected Android NDK version $ndk_releaseno"

c_path=""
cxx_path=""
case "$ndk_releaseno" in
    8e|"8e (64-bit)")
        c_path=$ndk_root/toolchains/$toolchain/prebuilt/$platform/bin/$ccompiler
        cxx_path=$ndk_root/toolchains/$toolchain/prebuilt/$platform/bin/$cxxcompiler
        ;;
    *)
        echo "Unrecognised NDK release '$ndk_releaseno'. Please update make-prefix-header.sh."
        exit 1
        ;;
esac

echo "$c_path -x c-header $gcc_toolchain -std=gnu11 -I./../../ePub3 -I$ndk_root/platforms/android-9/arch-$1/usr/include prefix.h -o prefix.h.pch"
$cxx_path -x c-header $gcc_toolchain -std=gnu11 -I./../../ePub3 -I$ndk_path/platforms/android-9/arch-$1/usr/include -I$ndk_path/sources/cxx-stl/gnu-libstdc++/$gnuvers/include prefix.h -o prefix.h.pch

echo "$cxx_path -x c++-header $gcc_toolchain -std=gnu++11 -I./../../ePub3 -I$ndk_root/platforms/android-9/arch-$1/usr/include -I$ndk_root/sources/cxx-stl/gnu-libstdc++/$gnuvers/include prefix.h -o prefix.h.pch++"
$cxx_path -x c++-header $gcc_toolchain -std=gnu++11 -I./../../ePub3 -I$ndk_path/platforms/android-9/arch-$1/usr/include -I$ndk_path/sources/cxx-stl/gnu-libstdc++/$gnuvers/include prefix.h -o prefix.h.pch++

