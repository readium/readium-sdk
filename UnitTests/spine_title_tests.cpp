//
//  spine_title_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/20/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/nav_table.h"
#include "../ePub3/utilities/error_handler.h"
#include "catch.hpp"

#define EPUB_PATH "TestData/alice3.epub"

using namespace ePub3;

static ContainerPtr GetContainer()
{
    static ContainerPtr __c;
    static std::once_flag __once;
    std::call_once(__once, [&]() {
        SetErrorHandler([](const error_details& err) {
            if (err.epub_error_code() == EPUBError::OPFMissingModificationDateMetadata)
                return true;
            return DefaultErrorHandler(err);
        });
        __c = Container::OpenContainer(EPUB_PATH);
        SetErrorHandler(DefaultErrorHandler);
    });
    return __c;
}

TEST_CASE("SpineItems should have titles when they match a TOC item", "[toc]")
{
    ContainerPtr container = GetContainer();
    auto pkg = container->DefaultPackage();
    auto spine = pkg->FirstSpineItem();
    
    // in alice3.epub, the first item isn't in the TOC, and has an empty title
    // the full list should be:
    /*
     - ""
     - "I. Down the Rabbit-Hole"
     - "II. The Pool of Tears"
     - "III. A Caucus-Race and a Long Tale"
     - "IV. The Rabbit Sends in a Little Bill"
     - "V. Advice from a Caterpillar"
     - "VI. Pig and Pepper"
     - "VII. A Mad Tea-Party"
     - "VIII. The Queen's Croquet-Ground"
     - "IX. The Mock Turtle's Story"
     - "X. The Lobster Quadrille"
     - "XI. Who Stole the Tarts?"
     - "XII. Alice's Evidence"
     */
    std::vector<string> titles = {
        "",
        "I. Down the Rabbit-Hole",
        "II. The Pool of Tears",
        "III. A Caucus-Race and a Long Tale",
        "IV. The Rabbit Sends in a Little Bill",
        "V. Advice from a Caterpillar",
        "VI. Pig and Pepper",
        "VII. A Mad Tea-Party",
        "VIII. The Queen's Croquet-Ground",
        "IX. The Mock Turtle's Story",
        "X. The Lobster Quadrille",
        "XI. Who Stole the Tarts?",
        "XII. Alice's Evidence"
    };
    
    for (auto& title : titles)
    {
        REQUIRE(spine->Title() == title);
        spine = spine->Next();
    }
}
