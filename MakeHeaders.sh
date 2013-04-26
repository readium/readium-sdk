#!/bin/bash
#
#  make-prefix-header.sh
#  ePub3
#
#  Created by Jim Dovey on 2013-04-26.
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

if [ "$1" = "" ]; then
    echo "Usage: MakeHeaders.sh <Apple|Android|Windows>"
    echo "Example:"
    echo "    MakeHeaders.sh Apple"
    exit 1
fi

if [ "$1" = "Windows" ]; then
    cscript MakeHeaders.js
    exit $?
fi

Platform/"$1"/MakeHeaders.sh
exit $?