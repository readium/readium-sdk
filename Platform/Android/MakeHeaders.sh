#!/bin/zsh
#
#  MakeHeaders.sh
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

local include_dir=`dirname $0`/include
mkdir -p "$include_dir"

typeset -A groups
groups[boost]=`ls -d1 ePub3/ThirdParty/boost/include/boost/*`
groups[libxml]=`ls -d1 ePub3/ThirdParty/libxml2-android/include/libxml/*`
groups[openssl]=`ls -d1 ePub3/ThirdParty/openssl-android/include/openssl/*`
groups[utf8]=`ls -d1 ePub3/ThirdParty/utf8-cpp/include/*`
groups[google-url]=`ls -1 ePub3/ThirdParty/google-url/{base,src}/*.h`
groups[libzip]=`ls -1 ePub3/ThirdParty/libzip/*.h`
groups[ePub3/xml]=`ls -1 ePub3/xml/**/*.h`
groups[ePub3/utilities]=`ls -1 ePub3/utilities/*.{h,inl}`
groups[ePub3]=`ls -1 ePub3/*.h && ls -1 ePub3/ePub/*.h`

for key in ${(k)groups}
do
    value=$groups[$key]
    files=("${(f)value}")
    
    idir="$include_dir/$key"
    mkdir -p $idir
    
    for file in $files
    do
        cp -fRpv "$file" "$idir/"
        #rsync -r "$file" "$idir/"
    done
done