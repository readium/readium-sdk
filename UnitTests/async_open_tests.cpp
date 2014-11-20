//
//  async_open_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "catch.hpp"

using namespace ePub3;

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"

TEST_CASE("opening synchronously", "")
{
    ContainerPtr container = Container::OpenContainer(EPUB_PATH);
    REQUIRE(bool(container));
}

