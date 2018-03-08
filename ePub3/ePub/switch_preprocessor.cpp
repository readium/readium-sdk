//
//  switch_preprocessor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
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

#include "switch_preprocessor.h"
#include "package.h"
#include "container.h"
#include "filter_manager.h"
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

static const REGEX_NS::regex_constants::syntax_option_type SwitchRegexFlags = REGEX_NS::regex::icase|REGEX_NS::regex::optimize|REGEX_NS::regex::ECMAScript;

REGEX_NS::regex SwitchPreprocessor::CommentedSwitchIdentifier("(?:<!--)(\\s*<(?:epub:)switch(?:.|\\n|\\r)*?<(?:epub:)default(?:.|\\n|\\r)*?>\\s*)(?:-->)((?:.|\\n|\\r)*?)(?:<!--)(\\s*</(?:epub:)default>(?:.|\\n|\\r)*?)(?:-->)", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::SwitchContentExtractor("<(?:epub:)?switch(?:.|\\n|\\r)*?>((?:.|\\n|\\r)*?)</(?:epub:)?switch(?:.|\\n|\\r)*?>", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::CaseContentExtractor("<(?:epub:)?case\\s+required-namespace=\"(.*?)\">((?:.|\\n|\\r)*?)</(?:epub:)?case(?:.|\\n|\\r)*?>", SwitchRegexFlags);
REGEX_NS::regex SwitchPreprocessor::DefaultContentExtractor("<(?:epub:)?default(?:.|\\n|\\r)*?>((?:.|\\n|\\r)*?)</(?:epub:)?default(?:.|\\n|\\r)*?>", SwitchRegexFlags);

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
SwitchPreprocessor::NamespaceList SwitchPreprocessor::_supportedNamespaces{};
#else
static const string __default_namespaces[1] = {MathMLNamespaceURI};
SwitchPreprocessor::NamespaceList SwitchPreprocessor::_supportedNamespaces(&__default_namespaces[0], &__default_namespaces[1]);
#endif

bool SwitchPreprocessor::SniffSwitchableContent(ConstManifestItemPtr item)
{
    return (item->MediaType() == "application/xhtml+xml" && item->HasProperty(ItemProperties::ContainsSwitch));
}
ContentFilterPtr SwitchPreprocessor::SwitchFilterFactory(ConstPackagePtr package)
{
    for ( auto& item : package->Manifest() )
    {
        if ( item.second->HasProperty(ItemProperties::ContainsSwitch) )
            return std::make_shared<SwitchPreprocessor>(); //New();
    }
    return nullptr;
}
void SwitchPreprocessor::Register()
{
    FilterManager::Instance()->RegisterFilter("SwitchPreprocessor", SwitchStaticHandling, SwitchFilterFactory);
}
void * SwitchPreprocessor::FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen)
{
    char* input = reinterpret_cast<char*>(data);
    
    // handle partially-commented switch statements
    std::string inputStr(reinterpret_cast<const char*>(data));
    std::string replacement("$1$2$3");
    std::string str = REGEX_NS::regex_replace(inputStr, CommentedSwitchIdentifier, replacement);
    
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
