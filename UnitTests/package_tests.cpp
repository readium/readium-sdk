//
//  package_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-02.
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

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "catch.hpp"
#include <cstdlib>

#define EPUB_PATH "/Users/jdovey/Desktop/ePubsForMike/Dune-glossary.epub"

using namespace ePub3;

TEST_CASE("Package should have a Unique ID, Type, Version, and a Base Path", "")
{
    Container c(EPUB_PATH);
    Package* pkg = c.Packages()[0];
    
    REQUIRE_FALSE(pkg->UniqueID().empty());
    REQUIRE_FALSE(pkg->Type().empty());
    REQUIRE_FALSE(pkg->Version().empty());
    REQUIRE_FALSE(pkg->BasePath().empty());
}

TEST_CASE("Package should have multiple manifest items, and they should be indexable by identifier string", "")
{
    Container c(EPUB_PATH);
    auto pkg = c.Packages()[0];
    auto manifest = pkg->Manifest();
    REQUIRE_FALSE(manifest.empty());
    
    int idx = static_cast<int>(arc4random() % manifest.size());
    const ManifestItem* randomItem = nullptr;
    for ( auto pos = manifest.begin(); idx >= 0; ++pos, --idx )
    {
        randomItem = pos->second;
    }
    
    auto subscripted = (*pkg)[randomItem->Identifier()];
    REQUIRE(subscripted == randomItem);
    
    auto fetched = pkg->ManifestItemWithID(randomItem->Identifier());
    REQUIRE(fetched == randomItem);
}

TEST_CASE("Package should have multiple spine items", "")
{
    Container c(EPUB_PATH);
    auto pkg = c.Packages()[0];
    auto spine = pkg->FirstSpineItem();
    REQUIRE(spine != nullptr);
    
    size_t count = spine->Count();
    REQUIRE(count > 1);
    
    size_t idx = arc4random() % count;
    REQUIRE(pkg->SpineItemAt(idx) == (*pkg)[idx]);
}

TEST_CASE("Package should be able to create and resolve basic CFIs", "")
{
    Container c(EPUB_PATH);
    auto pkg = c.Packages()[0];
    size_t spineIdx = arc4random() % pkg->FirstSpineItem()->Count();
    auto spineItem = pkg->SpineItemAt(spineIdx);
    
    // create a mutable CFI
    CFI cfi(pkg->CFIForSpineItem(spineItem));
    REQUIRE_FALSE(cfi.Empty());
    
    std::string str(_Str("/", pkg->SpineCFIIndex(), "/", spineIdx*2, "[", spineItem->Idref(), "]!"));
    REQUIRE(cfi.String() == _Str("epubcfi(", str, ")"));
    REQUIRE(cfi == _Str("epubcfi(", str, ")"));
    
    const ManifestItem* manifestItem = pkg->ManifestItemForCFI(cfi, nullptr);
    REQUIRE(manifestItem != nullptr);
    REQUIRE(manifestItem == spineItem->ManifestItem());
    
    // subpath returned does not have a leading forward-slash
    REQUIRE(pkg->CFISubpathForManifestItemWithID(manifestItem->Identifier()) == str.substr(1));
    
    CFI fragment("/4/2,1:22,1:28");
    cfi += fragment;
    
    CFI remainder;
    (void) pkg->ManifestItemForCFI(cfi, &remainder);
    REQUIRE(remainder == fragment);
}
