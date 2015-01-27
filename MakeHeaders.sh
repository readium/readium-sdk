#!/bin/bash
#
#  make-prefix-header.sh
#  ePub3
#
#  Created by Jim Dovey on 2013-04-26.
#
#  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
#  
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
#  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
#  
#  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this 
#  notice, Readium Foundation reserves the right to license this material under a different 
#  separate license, and if you have done so, the terms of that separate license control and 
#  the following references to GPL do not apply).
#  
#  This program is free software: you can redistribute it and/or modify it under the terms 
#  of the GNU Affero General Public License as published by the Free Software Foundation, 
#  either version 3 of the License, or (at your option) any later version. You should have 
#  received a copy of the GNU Affero General Public License along with this program.  If not, 
#  see <http://www.gnu.org/licenses/>.


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