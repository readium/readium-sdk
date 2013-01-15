//
//  string_tests.cpp
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

#include "../ePub3/xml/utilities/xmlstring.h"
#include "catch.hpp"

using ePub3::xml::string;

TEST_CASE("string conversions", "Strings should convert between multibyte encodings properly")
{
    // C++11 string constants
    const char     * c_utf8  = u8"This is a test string with a Unicode character: \u2026";
    const char16_t * c_utf16 =  u"This is a test string with a Unicode character: \u2026";
    const char32_t * c_utf32 =  U"This is a test string with a Unicode character: \U00002026";
    
    std::vector<string> vec{string(c_utf8), string(c_utf16), string(c_utf32)};
    for ( auto pos = vec.begin(); pos != vec.end(); ++pos )
    {
        SCOPED_INFO("Testing string at index " << std::distance(vec.begin(), pos));
        for ( auto pos2 = vec.begin(); pos2 != vec.end(); ++pos2 )
        {
            if ( pos == pos2 )
                continue;
            SCOPED_INFO("Comparing against string at index " << std::distance(vec.begin(), pos2));
            SCOPED_INFO("Strings are '" << pos->c_str() << "' and '" << pos2->c_str() << "'");
            REQUIRE(pos->compare(*pos2) == 0);
        }
        
        REQUIRE(pos->compare(c_utf8)  == 0);
        REQUIRE(pos->compare(c_utf16) == 0);
        REQUIRE(pos->compare(c_utf32) == 0);
    }œ
}

TEST_CASE("string searching", "Searches for UTF characters/substrings should return UTF-32 char indices")
{
    string str(u8"Test string\u2026 with many characters.");
    REQUIRE(str.find("with") == 13);
    REQUIRE(str.find(char32_t(0x2026)) == 11);
    REQUIRE(str.find(u"\u2026") == 11);
    REQUIRE(str.find(U"with") == 13);
}

TEST_CASE("string charset searching", "find_first_of with a set containing utf-8 multibyte characters should match by UTF code-point, not by byte")
{
    string str(u8"\u2026\u2033");   // "\xe2\x80\xa6\xe2\x80\xb3"
    
    REQUIRE(str.size() == 2);
    REQUIRE(str.find_first_of(char16_t(0x2033)) == 1);
    REQUIRE(str.find_first_of(u8"\u2033") == 1);
    REQUIRE(str.find_first_of("#$%") == string::npos);
}
