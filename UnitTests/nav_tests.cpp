//
//  ncx_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/8/2013.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/nav_table.h"
#include "catch.hpp"

#define EPUB2_PATH "TestData/dante-hell.epub"
#define EPUB3_PATH "TestData/childrens-literature-20120722.epub"

using namespace ePub3;

static ContainerPtr GetEPUB2Container()
{
    static ContainerPtr __c;
    static std::once_flag __once;
    std::call_once(__once, [&]() {
        __c = Container::OpenContainer(EPUB2_PATH);
    });
    return __c;
}

static ContainerPtr GetEPUB3Container()
{
    static ContainerPtr __c;
    static std::once_flag __once;
    std::call_once(__once, [&]() {
        __c = Container::OpenContainer(EPUB3_PATH);
    });
    return __c;
}

static void PrintNavElement(NavigationElementPtr nav, size_t indent)
{
    std::string prefix(indent, ' ');
    prefix += "- ";
    
    std::cerr << prefix << nav->Title();
    
    NavigationPointPtr point = std::dynamic_pointer_cast<NavigationPoint>(nav);
    if (point != nullptr)
    {
        string content = point->Content();
        if (content.size() > 20)
        {
            content.erase(20);
            content.append(u"…");
        }
        
        std::cerr << " (" << content << ")";
    }
    
    std::cerr << std::endl;
    
    for (auto& child : nav->Children())
    {
        PrintNavElement(child, indent+1);
    }
}

static void PrintNavTable(NavigationTablePtr nav)
{
    std::cerr << "Title: " << nav->Title() << std::endl;
    std::cerr << "Type:  " << nav->Type()  << std::endl;
    
    for (auto& child : nav->Children())
    {
        PrintNavElement(child, 1);
    }
}

TEST_CASE("ePub2 files should have a Table of Contents", "")
{
    PackagePtr pkg = GetEPUB2Container()->DefaultPackage();
    auto toc = pkg->TableOfContents();
    REQUIRE(bool(toc));
    
    //PrintNavTable(toc);
}

TEST_CASE("ePub3 files should have a Table of Contents", "")
{
    PackagePtr pkg = GetEPUB3Container()->DefaultPackage();
    auto toc = pkg->TableOfContents();
    REQUIRE(bool(toc));
    
    //PrintNavTable(toc);
}
