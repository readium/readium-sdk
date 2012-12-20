//
//  locator.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
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
