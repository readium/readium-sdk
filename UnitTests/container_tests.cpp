//
//  container_tests.cpp
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
#include "catch.hpp"

using namespace ePub3;

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"

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
    Container container(EPUB_PATH);
    REQUIRE(container.Version() == "1.0");
}
