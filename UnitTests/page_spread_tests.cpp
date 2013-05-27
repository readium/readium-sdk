//
//  page_spread_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-27.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include <ePub3/container.h>
#include <ePub3/package.h>
#include <ePub3/spine.h>
#include "catch.hpp"

#define EPUB_PATH "TestData/page-blanche.epub"

using namespace ePub3;

TEST_CASE("Page spread comes back correctly", "")
{
    ContainerPtr c = Container::OpenContainer(EPUB_PATH);
    PackagePtr pkg = c->DefaultPackage();
    
    SpineItemPtr item = pkg->FirstSpineItem();
    REQUIRE(item->Spread() == PageSpread::Right);
    
    item = item->Next();
    REQUIRE(item->Spread() == PageSpread::Left);
}
