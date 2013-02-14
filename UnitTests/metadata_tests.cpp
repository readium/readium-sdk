//
//  metadata_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-24.
//  Copyright (c) 2013 The Readium Foundation.
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
#include "../ePub3/ePub/metadata.h"
#include "../ePub3/utilities/iri.h"
#include "catch.hpp"
#include <type_traits>

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"
#define LOCALIZED_EPUB_PATH "TestData/kusamakura-japanese-vertical-writing-20121124.epub"

using namespace ePub3;

static const Container* GetContainer()
{
    static Container* __c = nullptr;
    if ( __c == nullptr )
        __c = new Container(EPUB_PATH);
    return __c;
}

TEST_CASE("Package should have metadata", "")
{
    const Package* pkg = GetContainer()->Packages()[0];
    REQUIRE(pkg->Metadata().size() != 0);
}

TEST_CASE("Metadata should be identified by property IRIs", "")
{
    const Package::MetadataMap& metadata = GetContainer()->Packages()[0]->Metadata();
    for ( auto item : metadata )
    {
        REQUIRE(typeid(item->Property()) == typeid(IRI));
        /*
        // printout values for inspection
        for ( auto valuePair : item->DebugValues() )
        {
            fprintf(stdout, "%s: %s\n", valuePair.first.c_str(), valuePair.second.c_str());
        }
         */
    }
}

TEST_CASE("Title(), Subtitle(), and FullTitle() should work as expected", "")
{
    const Package* pkg = GetContainer()->Packages()[0];
    REQUIRE(pkg->Title() == "Children's Literature");
    REQUIRE(pkg->Subtitle() == "A Textbook of Sources for Teachers and Teacher-Training Classes");
    REQUIRE(pkg->FullTitle() == "Children's Literature: A Textbook of Sources for Teachers and Teacher-Training Classes");
}

TEST_CASE("Author details should be accessible in different ways", "")
{
    const Package* pkg = GetContainer()->Packages()[0];
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
    const Package* pkg = GetContainer()->Packages()[0];
    REQUIRE(pkg->Language() == "en");
    REQUIRE(pkg->Source() == "http://www.gutenberg.org/files/25545/25545-h/25545-h.htm");
    REQUIRE(pkg->CopyrightOwner() == "Public domain in the USA.");
    REQUIRE(pkg->ModificationDate() == "2010-02-17T04:39:13Z");
}

TEST_CASE("An appropriately localized value should be returned if available", "")
{
    Container c(LOCALIZED_EPUB_PATH);
    const Package* pkg = c.Packages()[0];
    
    REQUIRE(pkg->Title() == u8"草枕");
    REQUIRE(pkg->Authors() == u8"夏目 漱石");
    REQUIRE(pkg->Contributors() == u8"柴田 卓治, 伊藤 時也, 総務省, EPUB日本語拡張仕様策定プロジェクト, 持田 怜香, 濱田 麻邑, 川幡 太一, and 村田 真");
    
    Package::SetLocale("en_US.UTF-8");
    REQUIRE(pkg->Title() == "Kusamakura");
    REQUIRE(pkg->Authors() == "Natsume, Sōseki");
    REQUIRE(pkg->Contributors() == u8"柴田 卓治, 伊藤 時也, Ministry of Internal Affairs and Communications, Japanese EPUB Specification Settlement Project, Reika Mochida, Mayu Hamada, Taichi Kawabata, and Makoto Murata");
}
