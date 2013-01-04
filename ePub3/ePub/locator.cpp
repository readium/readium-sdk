//
//  locator.cpp
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

#include "locator.h"
#include <iostream>

EPUB3_BEGIN_NAMESPACE

Locator::~Locator()
{
    if ( _reader != nullptr )
        delete _reader;
    if ( _writer != nullptr )
        delete _writer;
}

NullLocator::NullLocator() : Locator()
{
    // create the null streams
    _reader = new std::istream(nullptr);
    _writer = new std::ostream(nullptr);
}

EPUB3_END_NAMESPACE
