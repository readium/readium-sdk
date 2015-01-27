//
//  cfi_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-04.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePub3/ePub/cfi.h"
#include "../ePub3/utilities/error_handler.h"
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
    REQUIRE_NOTHROW(CFI(u8"epubcfi(/6/16[夏目漱石]!)"));        // utf-8
    REQUIRE_NOTHROW(CFI(u"epubcfi(/6/16[夏目漱石]!)"));         // utf-16
    REQUIRE_NOTHROW(CFI(U"epubcfi(/6/16[夏目漱石]!)"));         // utf-32
    
    REQUIRE_NOTHROW(CFI("epubcfi(/6/4[chap01]!/4/52/3:22[;s=b])"));
    REQUIRE(CFI("epubcfi(/6/4[chap01]!/4/52/3:22[;s=b])").CharacterSideBias() == CFI::SideBias::Before);
    
    // invalid strings
    REQUIRE_THROWS_AS(CFI("6/4"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("/:22"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("epubcfi/6"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("epubcfi()"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI(""), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("Henry Fitzwilliam"), epub_spec_error);
    
    EPUBError expectedErr = EPUBError::NoError;
    SetErrorHandler([&](const error_details& err){
        if ( err.is_spec_error() && err.epub_error_code() == expectedErr )
            return false;
        return true;
    });
    
    expectedErr = EPUBError::CFIRangeComponentCountInvalid;
    REQUIRE_THROWS_AS(CFI("/6/4!,1:22"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("/6/4!,/1:22,"), epub_spec_error);
    
    // these should fail, but CFI does not enforce ordering of range delimiters right now
    expectedErr = EPUBError::CFIRangeInvalid;
    REQUIRE_THROWS_AS(CFI("epubcfi(/6/4[chap01]!/4/52,/5:12,/3:22)"), epub_spec_error);
    REQUIRE_THROWS_AS(CFI("epubcfi(/6/4[chap01]!/4/52,/5:12,/5:10)"), epub_spec_error);
    
    // no side-bias on ranges
    expectedErr = EPUBError::CFIRangeContainsSideBias;
    REQUIRE_THROWS_AS(CFI("/6/4[chap01]!/4/52,/3:22,/5/12:3[;s=a]"), epub_spec_error);
    
    SetErrorHandler(DefaultErrorHandler);
    
    // are these forms valid or invalid?
    // "epubcfi(/6/4!/4/2/2:20)" -- char index on even-numbered node: is this equivalent to someTag/text():20 ?
}

TEST_CASE("Location CFIs should be appendable using valid CFIs and strings; Range CFIs should not", "")
{
    CFI base("/6/4!");
    
    REQUIRE_NOTHROW(base + CFI("/2/3:5"));
    REQUIRE_NOTHROW(base + string("/2/3:5"));
    REQUIRE_THROWS_AS(base + CFI(":25"), epub_spec_error);
    REQUIRE_THROWS_AS(base + string(":25"), epub_spec_error);
    
    REQUIRE_NOTHROW(base += CFI("/2,/3:5,/3:8"));
    REQUIRE(base.IsRangeTriplet());
    
    REQUIRE_THROWS_AS(base + CFI("/5/3:2"), CFI::RangedCFIAppendAttempt);
    REQUIRE_THROWS_AS(base + string("/5/3:2"), CFI::RangedCFIAppendAttempt);
}

TEST_CASE("Location and Range CFIs should be reassignable by CFI or string, even between types", "")
{
    CFI base("/6/4");
    
    REQUIRE_NOTHROW(base = "/6/6!");
    REQUIRE_THROWS_AS(base = "2", epub_spec_error);
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
