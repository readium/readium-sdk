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

[[ $# == 0 || $# > 2 ]] && {
    SHOW_HELP="true"
} || {
    COMMAND="$1"
}

NDK_PATH="$ANDROID_NDK"
[[ "$NDK_PATH" == "" && $# == 2 ]] && {
    NDK_PATH="$2"
} || [[ "$NDK_PATH" == "" ]] && {
    SHOW_HELP="true"
}

[[ "$COMMAND" == "help" ]] && {
    SHOW_HELP="true"
}

[[ "$SHOW_HELP" == "true" ]] && {
    echo "usage: ndk-compile.sh <command> [<path-to-ndk>]"
    echo "       Available commands are:"
    echo "           help           Shows this help message."
    echo "           build          Builds this native project."
    echo "           build-debug    Builds this native project with debug enabled."
    echo "           clean          Cleans this project build objects."
    echo ""
    echo "       <path-to-ndk>  The path to the Android NDK to use."
    echo ""
    echo "       The path to NDK can be set by a global environment variable ANDROID_NDK,"
    echo "       to avoid being passed by argument to this script. "
    exit 1
}

PROJECT_PATH=`pwd`

echo "Starting ndk-build..."
echo ""
echo "Project path: $PROJECT_PATH"
echo "NDK path: $NDK_PATH"
echo "Command: $COMMAND"
echo ""

# Take care of includes

[[ "$COMMAND" == "clean" ]] && {
    rm -rf "$PROJECT_PATH/include"
} || {
    [[ -d "$PROJECT_PATH/include" ]] || {
        cd ../..
        sh ./MakeHeaders.sh Android
        cd -
    }
}


# Run build

case "$COMMAND" in
    "build")
        $NDK_PATH/ndk-build -C . V=1 NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH="$PROJECT_PATH" APP_BUILD_SCRIPT=Android.mk
        ;;
    "build-debug")
        $NDK_PATH/ndk-build -C . V=1 NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH="$PROJECT_PATH" APP_BUILD_SCRIPT=Android.mk NDK_DEBUG=1
        ;;
    "clean")
        $NDK_PATH/ndk-build clean -C . V=1 NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH="$PROJECT_PATH" APP_BUILD_SCRIPT=Android.mk
        ;;
esac
