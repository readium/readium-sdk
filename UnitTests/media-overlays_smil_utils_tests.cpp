//
//  media-overlays_smil_utils_tests.cpp
//  ePub3
//
//  Created by Daniel Weck on 2013-09-11.
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

//#include <string>
#include "../ePub3/utilities/utfstring.h"

#include "../ePub3/ePub/media-overlays_smil_utils.h"

#include "catch.hpp"

//using namespace ePub3;
using ePub3::string;

void testParseSmilClockValue(const string& str, uint32_t timeExpected)
{
    SCOPED_INFO("SMIL Clock Value: '" << str.c_str() << "' (expects: " << timeExpected << ")");
    REQUIRE_NOTHROW(ePub3::ParseSmilClockValueToMilliseconds(str));
    uint32_t timeObtained = ePub3::ParseSmilClockValueToMilliseconds(str);
    REQUIRE(timeObtained == timeExpected);
}

TEST_CASE
(
"SMIL Clock Values parsing",
"Character strings that represent SMIL time codes should be successfully converted to corresponding values in milliseconds units"
)
{
    // 2 hours, 30 minutes and 3 seconds
    // =2*60*60*1000+30*60*1000+3*1000+0
    testParseSmilClockValue("02:30:03", 9003000);

    // 50 hours, 10 seconds and 250 milliseconds
    // =50*60*60*1000+0*60*1000+10*1000+250
    testParseSmilClockValue("50:00:10.25", 180010250);

    // 2 minutes and 33 seconds
    // =0*60*60*1000+2*60*1000+33*1000+0
    testParseSmilClockValue("02:33", 153000);

    // 10.5 seconds = 10 seconds and 500 milliseconds
    // =0*60*60*1000+0*60*1000+10*1000+500
    testParseSmilClockValue("00:10.5", 10500);

    // 3.2 hours = 3 hours and 12 minutes
    // =3*60*60*1000+12*60*1000+0*1000+0
    testParseSmilClockValue("3.2h", 11520000);

    // 45 minutes
    // =0*60*60*1000+45*60*1000+0*1000+0
    testParseSmilClockValue("45min", 2700000);

    // 30 seconds
    // =0*60*60*1000+0*60*1000+30*1000+0
    testParseSmilClockValue("30s", 30000);

    // 5 milliseconds
    testParseSmilClockValue("5ms", 5);

    // 12 seconds and 467 milliseconds
    // =0*60*60*1000+0*60*1000+12*1000+467
    testParseSmilClockValue("12.467", 12467);

    // 500 milliseconds
    testParseSmilClockValue("00.5s", 500);

    // 5 milliseconds
    testParseSmilClockValue("00:00.005", 5);

    // 5 hours, 34 minutes, 31 seconds and 396 milliseconds
    // =5*60*60*1000+34*60*1000+31*1000+396
    testParseSmilClockValue("5:34:31.396", 20071396);

    // 124 hours, 59 minutes and 36 seconds
    // =124*60*60*1000+59*60*1000+36*1000+0
    testParseSmilClockValue("124:59:36", 449976000);

    // 5 minutes, 1 second and 200 milliseconds
    // =0*60*60*1000+5*60*1000+1*1000+200
    testParseSmilClockValue("0:05:01.2", 301200);

    // 4 seconds
    // =0*60*60*1000+0*60*1000+4*1000+0
    testParseSmilClockValue("0:00:04", 4000);

    // 9 minutes and 58 seconds
    // =0*60*60*1000+9*60*1000+58*1000+0
    testParseSmilClockValue("09:58", 598000);

    // 56 seconds and 780 milliseconds
    // =0*60*60*1000+0*60*1000+56*1000+780
    testParseSmilClockValue("00:56.78", 56780);

    // 76.2 seconds = 76 seconds and 200 milliseconds
    // =0*60*60*1000+0*60*1000+76*1000+200
    testParseSmilClockValue("76.2s", 76200);

    // 7.75 hours = 7 hours and 45 minutes
    // =7*60*60*1000+45*60*1000+0*1000+0
    testParseSmilClockValue("7.75h", 27900000);

    // 13 minutes
    // =0*60*60*1000+13*60*1000+0*1000+0
    testParseSmilClockValue("13min", 780000);

    // 2345 milliseconds
    // =0*60*60*1000+0*60*1000+0*1000+2345
    testParseSmilClockValue("2345ms", 2345);

    // 12 seconds and 345 milliseconds
    // =0*60*60*1000+0*60*1000+12*1000+345
    testParseSmilClockValue("12.345", 12345);
}
