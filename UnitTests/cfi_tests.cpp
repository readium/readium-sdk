//
//  cfi_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-04.
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
    REQUIRE_NOTHROW(CFI(u8"epubcfi(/6/16[夏目漱石]!)"));        // utf-8
    REQUIRE_NOTHROW(CFI(u"epubcfi(/6/16[夏目漱石]!)"));         // utf-16
    REQUIRE_NOTHROW(CFI(U"epubcfi(/6/16[夏目漱石]!)"));         // utf-32
    
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
    REQUIRE_NOTHROW(base + string("/2/3:5"));
    REQUIRE_THROWS_AS(base + CFI(":25"), CFI::InvalidCFI);
    REQUIRE_THROWS_AS(base + string(":25"), CFI::InvalidCFI);
    
    REQUIRE_NOTHROW(base += CFI("/2,/3:5,/3:8"));
    REQUIRE(base.IsRangeTriplet());
    
    REQUIRE_THROWS_AS(base + CFI("/5/3:2"), CFI::RangedCFIAppendAttempt);
    REQUIRE_THROWS_AS(base + string("/5/3:2"), CFI::RangedCFIAppendAttempt);
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
