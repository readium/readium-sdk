//
//  collection_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/29/2013.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePub3/ePub/epub_collection.h"
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
