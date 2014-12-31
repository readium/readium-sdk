//
//  page_spread_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-27.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include <ePub3/container.h>
#include <ePub3/package.h>
#include <ePub3/spine.h>
#include "catch.hpp"

#define LR_SPREAD_EPUB_PATH "TestData/page-blanche.epub"
#define NO_SPREAD_EPUB_PATH "TestData/cole-voyage-of-life-20120320.epub"

using namespace ePub3;

TEST_CASE("Page spread comes back correctly", "")
{
    ContainerPtr c = Container::OpenContainer(LR_SPREAD_EPUB_PATH);
    PackagePtr pkg = c->DefaultPackage();
    
    SpineItemPtr item = pkg->FirstSpineItem();
    REQUIRE(item->Spread() == PageSpread::Right);
    
    item = item->Next();
    REQUIRE(item->Spread() == PageSpread::Left);
}

TEST_CASE("Multiple spine item properties should work", "")
{
    ContainerPtr c = Container::OpenContainer(NO_SPREAD_EPUB_PATH);
    PackagePtr pkg = c->DefaultPackage();
    
    SpineItemPtr item = pkg->SpineItemWithIDRef("s1b");
    REQUIRE(bool(item));
    
    REQUIRE(item->PropertyMatching("layout", "rendition")->Value() == "pre-paginated");
    REQUIRE(item->PropertyMatching("orientation", "rendition")->Value() == "landscape");
    REQUIRE(item->PropertyMatching("spread", "rendition")->Value() == "none");
}
