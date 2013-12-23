//
//  iri_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-09-04.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "../ePub3/utilities/iri.h"
#include "catch.hpp"

using namespace ePub3;

TEST_CASE("IRIs chould be constructible from c-strings", "")
{
    REQUIRE_NOTHROW(IRI("http://www.readium.org"));
    REQUIRE_NOTHROW(IRI("http://www.readium.org/"));
    REQUIRE_NOTHROW(IRI("http://readium.org/"));
    REQUIRE_NOTHROW(IRI("http://www.readium.org/projects"));
    REQUIRE_NOTHROW(IRI("http://www.readium.org/projects?arg1=1&arg2=2"));
    REQUIRE_NOTHROW(IRI("http://www.readium.org/projects#readium-sdk"));
    REQUIRE_NOTHROW(IRI("http://www.readium.org/projects#readium-sdk?arg1=1&arg2=2"));
}

TEST_CASE("IRIs should be equal to their string input when printed", "")
{
    string str("http://www.readium.org/");
    
    IRI iri(str);
    REQUIRE(iri.IRIString() == str);
}

TEST_CASE("IRIs should correctly encode/decode Unicode characters", "")
{
    string str("http://www.readium.org/∂-in-my-side");
    
    IRI iri(str);
    REQUIRE(iri.IRIString() == str);
    REQUIRE(iri.URIString() != str);
}

TEST_CASE("IRIs should be buildable by component", "")
{
    // Test IRI
    IRI iri;
    
    // before using a new scheme, remember to REGISTER IT
    IRI::AddStandardScheme("scheme");
    
    // Test 1: Build 'scheme://user:pass@host:69/path/to/file?query#fragment'
    iri = ePub3::IRI();
    iri.SetScheme("scheme");
    iri.SetCredentials("user", "pass");
    iri.SetHost("host");
    iri.SetPort(69);
    iri.AddPathComponent("path/to/file");
    iri.SetQuery("query");
    iri.SetFragment("fragment");
    REQUIRE(iri.URIString() == "scheme://user:pass@host:69/path/to/file?query#fragment");
    
    // Test 2: Init from string 'scheme://host/path/to/file?query#fragment'
    iri = "scheme://host/path/to/file?query#fragment";
    // Test 3: Get the bits from last test
    REQUIRE(iri.Scheme() == "scheme");
    REQUIRE(iri.Host() == "host");
    REQUIRE(iri.Path() == "/path/to/file");
    REQUIRE(iri.Query() == "query");
    REQUIRE(iri.Fragment() == "fragment");
    
    // Test 4: Init from bits
    iri = ePub3::IRI("scheme", "host", "/path/to/file",
                     "query", "fragment");
    REQUIRE(iri.URIString() == "scheme://host/path/to/file?query#fragment");
    
    // Test 5: Get the bits back
    REQUIRE(iri.Scheme() == "scheme");
    REQUIRE(iri.Host() == "host");
    REQUIRE(iri.Path() == "/path/to/file");
    REQUIRE(iri.Query() == "query");
    REQUIRE(iri.Fragment() == "fragment");
    
    // Test 6: Build 'http://user:pass@www.telenet.com/index.php?q=10#bottom'
    iri = ePub3::IRI();
    iri.SetScheme("http");
    iri.SetCredentials("user", "pass");
    iri.SetHost("www.telenet.com");
    iri.AddPathComponent("index.php");
    iri.SetQuery("q=10");
    iri.SetFragment("bottom");
    REQUIRE(iri.URIString() == "http://user:pass@www.telenet.com/index.php?q=10#bottom");
    
    // Test 7: Init from string 'http://www.telenet.com:8080/index.php?q=10#bottom'
    iri = "http://www.telenet.com:8080/index.php?q=10#bottom";
    
    // Test 8: Get the bits from last test
    REQUIRE(iri.Scheme() == "http");
    REQUIRE(iri.Host() == "www.telenet.com");
    REQUIRE(iri.Port() == 8080);
    REQUIRE(iri.Path() == "/index.php");
    REQUIRE(iri.Query() == "q=10");
    REQUIRE(iri.Fragment() == "bottom");
}
