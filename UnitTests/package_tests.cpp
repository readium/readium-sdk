//
//  package_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-02.
//  Copyright (c) 2013 Kobo Inc. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "catch.hpp"
#include <cstdlib>

#define EPUB_PATH "/Users/jdovey/Desktop/ePubsForMike/Dune-glossary.epub"

using namespace ePub3;

TEST_CASE("", "Package should have a Unique ID, Type, Version, and a Base Path")
{
    Container c(EPUB_PATH);
    Package* pkg = c.Packages()[0];
    
    REQUIRE_FALSE(pkg->UniqueID().empty());
    REQUIRE_FALSE(pkg->Type().empty());
    REQUIRE_FALSE(pkg->Version().empty());
    REQUIRE_FALSE(pkg->BasePath().empty());
}

TEST_CASE("", "Package should have multiple manifest items, and they should be indexable by identifier string")
{
    Container c(EPUB_PATH);
    auto pkg = c.Packages()[0];
    auto manifest = pkg->Manifest();
    REQUIRE_FALSE(manifest.empty());
    
    size_t idx = arc4random() % manifest.size();
    const ManifestItem* randomItem = nullptr;
    for ( auto pos = manifest.begin(); idx != 0; ++pos, --idx )
    {
        randomItem = pos->second;
    }
    
    auto subscripted = (*pkg)[randomItem->Identifier()];
    REQUIRE(subscripted == randomItem);
    
    auto fetched = pkg->ManifestItemWithID(randomItem->Identifier());
    REQUIRE(fetched == randomItem);
}
