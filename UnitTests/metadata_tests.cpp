//
//  metadata_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-24.
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
#include "../ePub3/ePub/property.h"
#include "../ePub3/ePub/property_holder.h"
#include "../ePub3/ePub/property_extension.h"
#include "../ePub3/utilities/iri.h"
#include "catch.hpp"
#include <type_traits>

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"
#define LOCALIZED_EPUB_PATH "TestData/kusamakura-japanese-vertical-writing-20121124.epub"
#define FXL_EPUB_PATH "TestData/page-blanche.epub"

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

TEST_CASE("Fixed Layout content should be identified using rendition:layout", "")
{
    ContainerPtr c = Container::OpenContainer(FXL_EPUB_PATH);
    PackagePtr pkg = c->DefaultPackage();
    
    IRI iri = pkg->MakePropertyIRI("layout", "rendition");
    INFO("Expected a valid IRI, but got " << iri.IRIString().stl_str());
    REQUIRE_FALSE(iri.IsEmpty());
    
    REQUIRE(pkg->ContainsProperty(iri));
    
    PropertyHolder::PropertyList list = pkg->PropertiesMatching(iri);
    INFO("Got " << list.size() << " properties matching IRI " << iri.IRIString().stl_str());
    REQUIRE(list.size() == 1);
    
    INFO("Got propertpy value " << list[0]->Value().stl_str());
    REQUIRE(list[0]->Value() == "pre-paginated");
}

TEST_CASE("Missing metadata should not throw", "")
{
    PackagePtr pkg = GetContainer()->DefaultPackage();
    
    IRI iri;
    REQUIRE_NOTHROW(iri = pkg->MakePropertyIRI("layout", "rendition"));
    
    PropertyHolder::PropertyList list;
    REQUIRE_NOTHROW(list = pkg->PropertiesMatching(iri));
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
