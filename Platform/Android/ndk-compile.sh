#!/bin/bash
#
#  ndk-compile.sh
#  ePub3
#
#  Created by Jim Dovey on 2013-04-11.
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
	echo "Usage: ndk-compile.sh <build|clean> <path-to-ndk>"
	exit 1
fi

clean=""
if [ "$1" = "clean" ]; then
    rm -rf "`pwd`/include"
    clean="clean"
else
    cur=`pwd`
    cd ../..
    sh ./MakeHeaders.sh Android
    cd $cur
fi

$2/ndk-build $clean -C . V=1 NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=`pwd` APP_BUILD_SCRIPT=Android.mk
