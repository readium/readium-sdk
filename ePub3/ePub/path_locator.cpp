//
//  path_locator.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012-2013 The Readium Foundation.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "path_locator.h"
#include <fstream>

EPUB3_BEGIN_NAMESPACE

std::istream& PathLocator::ReadStream()
{
    if ( _reader == nullptr )
        _reader = new std::ifstream(_path);
    return *_reader;
}
std::ostream& PathLocator::WriteStream()
{
    if ( _writer == nullptr )
        _writer = new std::ofstream(_path);
    return *_writer;
}

EPUB3_END_NAMESPACE
