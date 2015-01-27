//
//  container_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-02.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePub3/ePub/container.h"
#include "catch.hpp"

using namespace ePub3;

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"

TEST_CASE("opening a container", "The container should open without problem")
{
    ContainerPtr pContainer = Container::OpenContainer(EPUB_PATH);
    REQUIRE(pContainer != nullptr);
}

TEST_CASE("", "container should contain at least one package")
{
    ContainerPtr container = Container::OpenContainer(EPUB_PATH);
    auto packages = container->PackageLocations();
    for ( auto pkgLoc : packages )
    {
        CAPTURE(pkgLoc);
    }
    
    REQUIRE(packages.size() > 0);
}

TEST_CASE("", "Version should be 1.0")
{
    ContainerPtr container = Container::OpenContainer(EPUB_PATH);
    REQUIRE(container->Version() == "1.0");
}
