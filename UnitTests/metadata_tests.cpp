//
//  metadata_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-24.
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

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/property.h"
#include "../ePub3/ePub/property_holder.h"
#include "../ePub3/ePub/property_extension.h"
#include "../ePub3/utilities/iri.h"
#include "catch.hpp"
#include <type_traits>

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"
#define LOCALIZED_EPUB_PATH "TestData/kusamakura-japanese-vertical-writing-20121124.epub"

using namespace ePub3;

static ContainerPtr GetContainer()
{
    static ContainerPtr __c;
    if ( !__c )
        __c = Container::OpenContainer(EPUB_PATH);
    return __c;
}

TEST_CASE("Package should have metadata", "")
{
    PackagePtr pkg = GetContainer()->DefaultPackage();
    REQUIRE(pkg->NumberOfProperties() != 0);
}

TEST_CASE("Metadata should be identified by property IRIs", "")
{
    PackagePtr pkg = GetContainer()->DefaultPackage();
    pkg->ForEachProperty([&](PropertyPtr item) {
        REQUIRE(typeid(item->PropertyIdentifier()) == typeid(IRI));
        /*
        // printout values for inspection
        for ( auto valuePair : item->DebugValues() )
        {
            fprintf(stdout, "%s: %s\n", valuePair.first.c_str(), valuePair.second.c_str());
        }
         */
    });
}

TEST_CASE("Title(), Subtitle(), and FullTitle() should work as expected", "")
{
    PackagePtr pkg = GetContainer()->Packages()[0];
    REQUIRE(pkg->Title() == "Children's Literature");
    REQUIRE(pkg->Subtitle() == "A Textbook of Sources for Teachers and Teacher-Training Classes");
    REQUIRE(pkg->FullTitle() == "Children's Literature: A Textbook of Sources for Teachers and Teacher-Training Classes");
}

TEST_CASE("Author details should be accessible in different ways", "")
{
    PackagePtr pkg = GetContainer()->Packages()[0];
    Package::AttributionList authorNames = { "Charles Madison Curry", "Erle Elsworth Clippinger" };
    Package::AttributionList authorAttrib = { "Curry, Charles Madison", "Clippinger, Erle Elsworth" };
    
    REQUIRE(pkg->AuthorNames() == authorNames);
    REQUIRE(pkg->AttributionNames() == authorAttrib);
    REQUIRE(pkg->Authors() == "Charles Madison Curry and Erle Elsworth Clippinger");
}

TEST_CASE("Subjects should be correct", "")
{
    Package::StringList expected({"Children -- Books and reading", "Children's literature -- Study and teaching"});
    REQUIRE(GetContainer()->Packages()[0]->Subjects() == expected);
}

TEST_CASE("Simple string metadata values should be correct", "")
{
    PackagePtr pkg = GetContainer()->Packages()[0];
    REQUIRE(pkg->Language() == "en");
    REQUIRE(pkg->Source() == "http://www.gutenberg.org/files/25545/25545-h/25545-h.htm");
    REQUIRE(pkg->CopyrightOwner() == "Public domain in the USA.");
    REQUIRE(pkg->ModificationDate() == "2010-02-17T04:39:13Z");
}

TEST_CASE("An appropriately localized value should be returned if available", "")
{
    ContainerPtr c = Container::OpenContainer(LOCALIZED_EPUB_PATH);
    PackagePtr pkg = c->Packages()[0];
    
    REQUIRE(pkg->Title() == u8"草枕");
    REQUIRE(pkg->Authors() == u8"夏目 漱石");
    REQUIRE(pkg->Contributors() == u8"柴田 卓治, 伊藤 時也, 総務省, EPUB日本語拡張仕様策定プロジェクト, 持田 怜香, 濱田 麻邑, 川幡 太一, and 村田 真");
    
    ePub3::SetCurrentLocale("en_US.UTF-8");
    REQUIRE(pkg->Title() == "Kusamakura");
    REQUIRE(pkg->Authors() == "Natsume, Sōseki");
    REQUIRE(pkg->Contributors() == u8"柴田 卓治, 伊藤 時也, Ministry of Internal Affairs and Communications, Japanese EPUB Specification Settlement Project, Reika Mochida, Mayu Hamada, Taichi Kawabata, and Makoto Murata");
}
