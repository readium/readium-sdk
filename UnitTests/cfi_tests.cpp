//
//  cfi_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-04.
//  Copyright (c) 2013 Kobo Inc. All rights reserved.
//

#include "../ePub3/ePub/cfi.h"
#include "catch.hpp"

using namespace ePub3;

TEST_CASE("CFIs should be constructable from valid strings", "")
{
    // valid strings
    REQUIRE_NOTHROW(CFI("/6/4[chap01]!"));
    REQUIRE_NOTHROW(CFI("/6/4[chap01]!/"));
    REQUIRE_NOTHROW(CFI("/6/4[chap01]!/4/52/3:22"));
    REQUIRE_NOTHROW(CFI("/6/4[chap01]!/4/52,/3:22,/5:12"));
    REQUIRE_NOTHROW(CFI("epubcfi(/6/4[chap01]!/4/52,/3:22,/5:12)"));
    
    // invalid strings
    REQUIRE_THROWS_AS(CFI("6/4"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("/:22"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("epubcfi/6"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("epubcfi()"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI(""), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("Henry Fitzwilliam"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("/6/4!,1:22"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("/6/4!,/1:22,"), CFI::InvalidCFI);
    
    // these should fail, but CFI does not enforce ordering of range delimiters right now
    REQUIRE_THROWS_AS(CFI("epubcfi(/6/4[chap01]!/4/52,/5:12,/3:22)"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(CFI("epubcfi(/6/4[chap01]!/4/52,/5:12,/5:10)"), CFI::InvalidCFI);
    
    // are these forms valid or invalid?
    // "epubcfi(/6/4!/4/2/2:20)" -- char index on even-numbered node: is this equivalent to someTag/text():20 ?
}

TEST_CASE("Location CFIs should be appendable using valid CFIs and strings; Range CFIs should not", "")
{
    CFI base("/6/4!");
    
    REQUIRE_NOTHROW(base + CFI("/2/3:5"));
    REQUIRE_NOTHROW(base + "/2/3:5");
    REQUIRE_THROWS_AS(base + CFI(":25"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(base + ":25", CFI::InvalidCFI);
    
    REQUIRE_NOTHROW(base += CFI("/2,/3:5,/3:8"));
    REQUIRE(base.IsRangeTriplet());
    
    REQUIRE_THROWS_AS(base + CFI("/5/3:2"), CFI::RangedCFIAppendAttempt);
    REQUIRE_THROWS_AS(base + "/5/3:2", CFI::RangedCFIAppendAttempt);
}

TEST_CASE("Location and Range CFIs should be reassignable by CFI or string, even between types", "")
{
    CFI base("/6/4");
    
    REQUIRE_NOTHROW(base = "/6/6!");
    REQUIRE_THROWS_AS(base = "2", CFI::InvalidCFI);
    REQUIRE(base == CFI("/6/6!"));
    REQUIRE(base == "epubcfi(/6/6!)");
    REQUIRE_FALSE(base == "bob");
    REQUIRE_FALSE(base == "epubcfi(/6/6!/4/3:2)");
    
    // switch to ranged type
    REQUIRE_NOTHROW(base = "/6/4!/4/2,/3:5,/5:8");
    REQUIRE(base.IsRangeTriplet());
    
    // switch back to location type
    REQUIRE_NOTHROW(base = "/6/4!/4/3:5");
    REQUIRE_FALSE(base.IsRangeTriplet());
}
