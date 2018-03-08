//
//  media-overlays_smil_utils.h
//  ePub3
//
//  Created by Daniel Weck on 2013-09-11.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ePub3_mo_smil_utils_h
#define ePub3_mo_smil_utils_h

#define USE_CHAR_STRING

#include <ePub3/base.h>

//#include <cmath>
#include <math.h>

#include <sstream>

//#include <string>
#include <ePub3/utilities/utfstring.h>

#include <ePub3/property_holder.h>

EPUB3_BEGIN_NAMESPACE

/**
 Parser for SMIL Clock Values

	See:
	http://www.w3.org/TR/SMIL3/smil-timing.html#Timing-ClockValueSyntax
	http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#app-clock-examples
	
	See also:
	http://www.w3.org/TR/smil-animation/#TimingAttrValGrammars
	http://dxr.mozilla.org/mozilla-central/source/content/smil/test/test_smilTiming.xhtml
	
 @remarks whitespace padding (i.e. at beginning and end of given string expression) is not permitted,
          so it must be trimmed on client side of this API, otherwise an "invalid character" exception will be thrown.
          The exception std::invalid_argument is thrown when an invalid character is encountered.
          Note: "0:0:0.5006" == "500.6ms"
 
 @see MediaOverlaysSmilModel for parsing a SMIL (XML) file into an in-memory data model
 
 @ingroup epub-model
 */
class SmilClockValuesParser
{
private:
    SmilClockValuesParser();

public:
    //EPUB3_EXPORT
    static
    uint32_t ToWholeMilliseconds(const string& str)
    {
        double fractional = ToSeconds(str) * 1000.0;
        uint32_t whole = (uint32_t) floor(fractional);
        return whole;
    }

    //EPUB3_EXPORT
    static
    uint32_t ToWholeNanoseconds(const string& str)
    {
        double fractional = ToSeconds(str) * 1000000.0;
        uint32_t whole = (uint32_t) floor(fractional);
        return whole;
    }

    //EPUB3_EXPORT
    static
    double ToSeconds(const string&
#ifdef USE_CHAR_STRING
            strstr
#else
            str
#endif
    )
    {
        if (
#ifdef USE_CHAR_STRING
            strstr
#else
            str
#endif
.length() == 0)
        {
            return 0;
        }

#ifdef USE_CHAR_STRING
        const char* str = (char*)strstr.c_str(); //&strstr[0];
#endif

        //printf("SMIL CLOCK VALUE STRING: %s\n", str.c_str());

        size_t index = 0; // left to right scan

        uint32_t d1 = parseDigits(str, &index);

        char current = str[index];

        double offset = 0.0;

        if (current == ':')
        {
            current = advanceChar(str, &index);

            int d2 = parseDigits(str, &index);

            current = str[index];

            if (current == ':')
            {
                current = advanceChar(str, &index);

                int d3 = parseDigits(str, &index);

                current = str[index];

                offset = d1 * 3600.0 + d2 * 60.0 + d3;
            }
            else
            {
                offset = d1 * 60.0 + d2;
            }

            if (current == '.')
            {
                current = advanceChar(str, &index);

                offset += parseFraction(str, &index);

                current = str[index];
            }
        }
        else if (current == '.')
        {
            current = advanceChar(str, &index);

            double val = parseFraction(str, &index) + d1;

            current = str[index];

            offset = val * parseUnit(str, &index);

            current = str[index];
        }
        else
        {
            offset = d1 * parseUnit(str, &index);

            current = str[index];
        }

        return offset;
    }

private:

    /*
    static
    double round(double number)
    {
        return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
    }
    */

    /*
    static
    double roundDecimalPlaces(double number, size_t decimalPlace)
    {
        const double divider = pow(10, decimalPlace); //3 => 1000: accuracy to 3 decimal places

        double rounded = (number < 0.0 ? ceil(number * divider - 0.5) : floor(number * divider + 0.5)) / divider;

        return rounded;
    }
     */

    static
    void checkDigit(char current,
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        if (current < '0' || current > '9')
        {
            std::stringstream s;
            s << "The character '" << current << "' at position [" << index << "] in string \"" << str << "\" is not a valid digit !";
            throw std::invalid_argument(s.str());

            //throw std::runtime_error(string(""));
        }
    }

    static
    void badChar(char current,
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        std::stringstream s;
        s << "The character '" << current << "' at position [" << index << "] in string \"" << str << "\" is not valid !";
        throw std::invalid_argument(s.str());
    }

    static
    char advanceChar(
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        return (*index) == (
#ifdef USE_CHAR_STRING
            strlen(str)
#else
            str.length()
#endif
- 1) ? '\0' : str[++(*index)];
    }

    static
    uint32_t parseDigits(
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        char current = str[*index];

        checkDigit(current, str, index);

        uint32_t value = 0;
        do
        {
            value = value * 10 + (current - '0');

            current = advanceChar(str, index);

        } while (current >= '0' && current <= '9');

        return value;
    }

    static
    double parseFraction(
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        char current = str[*index];

        checkDigit(current, str, index);

        double value = 0;

        double weight = 0.1;
        //size_t decimalPlace = 1;

        do
        {
            value += weight * (current - '0');

            //weight = roundDecimalPlaces(weight * 0.1, ++decimalPlace);
            weight *= 0.1;

            current = advanceChar(str, index);

        } while (current >= '0' && current <= '9');

        return value;
    }

    static
    double parseUnit(
#ifdef USE_CHAR_STRING
            const char*
#else
            const string&
#endif
            str, size_t *index)
    {
        char current = str[*index];

        if (current == 'h')
        {
            current = advanceChar(str, index);

            return 3600.0;
        }
        else if (current == 'm')
        {
            current = advanceChar(str, index);

            if (current == 'i')
            {
                current = advanceChar(str, index);

                if (current != 'n')
                {
                    badChar(current, str, index);
                }

                current = advanceChar(str, index);

                return 60.0;
            }
            else if (current == 's')
            {
                current = advanceChar(str, index);

                return 0.001;
            }
            else
            {
                badChar(current, str, index);
            }
        }
        else if (current == 's')
        {
            current = advanceChar(str, index);
        }

        return 1.0;
    }
};

EPUB3_END_NAMESPACE

#endif // ePub3_mo_smil_utils_h
