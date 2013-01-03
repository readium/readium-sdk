//
//  container_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-02.
//  Copyright (c) 2013 Kobo Inc. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "catch.hpp"

using namespace ePub3;

#define EPUB_PATH "/Users/jdovey/Desktop/ePubsForMike/Dune-glossary.epub"

TEST_CASE("opening a container", "The container should open without problem")
{
    Container *pContainer = new Container(EPUB_PATH);
    REQUIRE(pContainer != nullptr);
    delete pContainer;
}

TEST_CASE("", "container should contain at least one package")
{
    Container container(EPUB_PATH);
    auto packages = container.PackageLocations();
    for ( auto pkgLoc : packages )
    {
        CAPTURE(pkgLoc);
    }
    
    REQUIRE(packages.size() > 0);
}

TEST_CASE("", "Version should be 1.0")
{
    Container container("/Users/jdovey/Desktop/ePubsForMike/Dune-glossary.epub");
    REQUIRE(container.Version() == "1.0");
}
