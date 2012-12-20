//
//  path_locator.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
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
