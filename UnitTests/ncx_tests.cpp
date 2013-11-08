//
//  ncx_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/8/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/nav_table.h"
#include "catch.hpp"

#define EPUB_PATH "TestData/dante-hell.epub"

using namespace ePub3;

static ContainerPtr GetContainer()
{
    static ContainerPtr __c;
    std::once_flag __once;
    std::call_once(__once, [&]() {
        __c = Container::OpenContainer(EPUB_PATH);
    });
    return __c;
}

TEST_CASE("ePub2 files should have a Table of Contents", "")
{
    PackagePtr pkg = GetContainer()->DefaultPackage();
    REQUIRE(bool(pkg->TableOfContents()));
}
