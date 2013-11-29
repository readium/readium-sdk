//
//  collection_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/29/2013.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
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

#include "../ePub3/ePub/collection.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/container.h"
#include <array>
#include "catch.hpp"

#define EPUB_PATH "TestData/moby-dick-preview-collection.epub"

using namespace EPUB3_NAMESPACE;

static ContainerPtr GetContainer()
{
    static ContainerPtr __c;
    static std::once_flag __once;
    std::call_once(__once, [&]() {
        __c = Container::OpenContainer(EPUB_PATH);
    });
    return __c;
}

TEST_CASE("collections are accessible", "[package][collection]")
{
    ContainerPtr container = GetContainer();
    PackagePtr pkg = container->DefaultPackage();
    
    REQUIRE(pkg->Collections().empty() == false);
}

TEST_CASE("collections are accessible by role and special accessor", "[package][collection]")
{
    ContainerPtr container = GetContainer();
    PackagePtr pkg = container->DefaultPackage();
    
    REQUIRE(bool(pkg->CollectionWithRole("preview")) == true);
    REQUIRE(bool(pkg->PreviewCollection()) == true);
}

TEST_CASE("collection is parsed correctly", "[collection]")
{
    ContainerPtr container = GetContainer();
    PackagePtr pkg = container->DefaultPackage();
    
    CollectionPtr previewCollection = pkg->PreviewCollection();
    REQUIRE(previewCollection != nullptr);
    REQUIRE(previewCollection->Role() == Collection::PreviewRole);
    
    REQUIRE(previewCollection->LinkCount() == 4);
    REQUIRE(previewCollection->LinkAt(0)->Href() == "titlepage.xhtml");
    REQUIRE(previewCollection->LinkAt(1)->Href() == "chapter_001.xhtml");
    REQUIRE(previewCollection->LinkAt(2)->Href() == "chapter_002.xhtml");
    REQUIRE(previewCollection->LinkAt(3)->Href() == "chapter_003.xhtml");
    
    REQUIRE(previewCollection->ChildCollectionCount() == 1);
    
    CollectionPtr manifest = previewCollection->ChildCollectionWithRole(Collection::ManifestRole);
    REQUIRE(manifest != nullptr);
    REQUIRE(manifest->LinkCount() == 10);
    
    const char* expected[10][2] = {
        { "fonts/STIXGeneral.otf",              "application/vnd.ms-opentype" },
        { "fonts/STIXGeneralItalic.otf",        "application/vnd.ms-opentype" },
        { "fonts/STIXGeneralBol.otf",           "application/vnd.ms-opentype" },
        { "fonts/STIXGeneralBolIta.otf",        "application/vnd.ms-opentype" },
        { "images/Moby-Dick_FE_title_page.jpg", "image/jpeg" },
        { "css/stylesheet.css",                 "text/css" },
        { "titlepage.xhtml",                    "application/xhtml+xml" },
        { "chapter_001.xhtml",                  "application/xhtml+xml" },
        { "chapter_002.xhtml",                  "application/xhtml+xml" },
        { "chapter_003.xhtml",                  "application/xhtml+xml" }
    };
    
    for (int i = 0; i < 10; i++)
    {
        LinkPtr link = manifest->LinkAt(i);
        REQUIRE(link != nullptr);
        REQUIRE(link->Href() == expected[i][0]);
        REQUIRE(link->MediaType() == expected[i][1]);
        REQUIRE(link->Rel().empty());
    }
}
