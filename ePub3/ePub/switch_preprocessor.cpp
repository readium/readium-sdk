//
//  switch_preprocessor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
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

#include "switch_preprocessor.h"
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

static const REGEX_NS::regex_constants::syntax_option_type SwitchRegexFlags = REGEX_NS::regex::icase|REGEX_NS::regex::optimize|REGEX_NS::regex::ECMAScript;

REGEX_NS::regex SwitchPreprocessor::CommentedSwitchIdentifier(R"X((?:<!--)(\s*<(?:epub:)switch(?:.|\n|\r)*?<(?:epub:)default(?:.|\n|\r)*?>\s*)(?:-->)((?:.|\n|\r)*?)(?:<!--)(\s*</(?:epub:)default>(?:.|\n|\r)*?)(?:-->))X", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::SwitchContentExtractor(R"X(<(?:epub:)?switch(?:.|\n|\r)*?>((?:.|\n|\r)*?)</(?:epub:)?switch(?:.|\n|\r)*?>)X", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::CaseContentExtractor(R"X(<(?:epub:)?case\s+required-namespace="(.*?)">((?:.|\n|\r)*?)</(?:epub:)?case(?:.|\n|\r)*?>)X", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::DefaultContentExtractor(R"X(<(?:epub:)?default(?:.|\n|\r)*?>((?:.|\n|\r)*?)</(?:epub:)?default(?:.|\n|\r)*?>)X", SwitchRegexFlags);

bool SwitchPreprocessor::SniffSwitchableContent(const ManifestItem *item, const EncryptionInfo *encInfo __unused)
{
    return (item->MediaType() == "application/xhtml+xml" && item->HasProperty(ItemProperties::ContainsSwitch));
}
void * SwitchPreprocessor::FilterData(void *data, size_t len, size_t *outputLen)
{
    char* input = reinterpret_cast<char*>(data);
    
    // handle partially-commented switch statements
    std::string inputStr(reinterpret_cast<const char*>(data));
    std::string str = REGEX_NS::regex_replace(inputStr, CommentedSwitchIdentifier, "$1$2$3");
    
    // str now contains a non-commented value (or the input if no switch statements were commented out)
    auto pos = REGEX_NS::sregex_iterator(str.begin(), str.end(), SwitchContentExtractor);
    auto end = REGEX_NS::sregex_iterator();
    
    std::string output;
    while ( pos != end )
    {
        // output any non-matched characters
        output += pos->prefix();
        
        std::string switchContents = pos->str(1);
        
        bool matched = false;
        
        // now scan the contents for any epub:case statements
        if ( _supportedNamespaces.size() != 0 )
        {
            auto cpos = REGEX_NS::sregex_iterator(switchContents.begin(), switchContents.end(), CaseContentExtractor);
            while ( cpos != end )
            {
                for ( auto ns : _supportedNamespaces )
                {
                    if ( ns == cpos->str(1) )
                    {
                        // this case uses a supported namespace
                        matched = true;
                        break;
                    }
                }
                
                if ( matched )
                {
                    // we matched the case, so append its contents and skip the remainder
                    output += cpos->str(2);
                    break;
                }
                
                ++cpos;
            }
        }
        
        if ( !matched )
        {
            // no matching case statements, so find the epub:default
            REGEX_NS::smatch defaultCase;
            if ( REGEX_NS::regex_search(switchContents, defaultCase, DefaultContentExtractor) )
                output += defaultCase[1].str();
        }
        
        // step forward, and if we get the end let's output the current suffix
        // this ensures that we don't lose everything following the last match
        auto here = pos++;
        if ( pos == end )
            output += here->suffix();
    }
    
    *outputLen = output.size();
    if ( output.size() < len )
    {
        output.copy(input, output.size());
        return input;
    }
    
    char* result = new char[output.size()];
    output.copy(result, output.size());
    return result;
}

EPUB3_END_NAMESPACE
