//
//  css_preprocessor.cpp
//  ePub3
//
//  Created by Olivier Körner on 2016-03-08.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "css_preprocessor.h"
#include "package.h"
#include "filter_manager.h"

static const REGEX_NS::regex::flag_type regexFlags(REGEX_NS::regex::ECMAScript|REGEX_NS::regex::optimize|REGEX_NS::regex::icase);
//static const REGEX_NS::regex reEscaper("\\\\\\.\\(\\)\\[\\]\\$\\^\\*\\+\\?\\:\\=\\|", regexFlags);

EPUB3_BEGIN_NAMESPACE

static REGEX_NS::regex CSSMatcher("page\\-break\\-(after|before|inside) *\\: *(always|avoid|left|right)", regexFlags);
static REGEX_NS::regex StyleAttributeMatcher("<[^>]+style=\\\"([^\\\"]*)\"", regexFlags);
static REGEX_NS::regex StyleTagMatcher("<style[^>]*>((.|\n|\r)*?)<\\/style>", regexFlags);

static const std::string PageBreakReplacement ="-webkit-column-break-$1: $2";

//CSSFilterContext::CSSFilterContext(ConstManifestItemPtr item) {
//    this->setCSS(item->MediaType() == "text/css");
//}

bool CSSPreprocessor::ShouldApply(ConstManifestItemPtr item)
{
    auto mediaType = item->MediaType();
    bool itemPrepaginated = false;
    auto iprop = item->PropertyMatching("layout", "rendition");
    if (iprop != nullptr) {
        auto ilayout = iprop->Value();
        itemPrepaginated = (ilayout == "pre-paginated");
    }
    bool pkgPrepaginated = false;
    auto prop = item->GetPackage()->PropertyMatching("layout", "rendition");
    if (prop != nullptr) {
        auto layout = prop->Value();
        pkgPrepaginated = (layout == "pre-paginated");
    }
    
    if (itemPrepaginated || pkgPrepaginated)
        return false;
    
    //std::cout << "CSSPreprocessor ShouldApply " << mediaType << " " << item->Href();
    return (mediaType == "application/xhtml+xml" || mediaType == "text/html" || mediaType == "text/css");
}

ContentFilterPtr CSSPreprocessor::CSSFilterFactory(ConstPackagePtr package)
{
    CSSSubstitution pageBreakSub(CSSMatcher, PageBreakReplacement);
    std::vector<CSSSubstitution> substitutions { pageBreakSub };
    return New(package, substitutions);
}

void CSSPreprocessor::Register()
{
    FilterManager::Instance()->RegisterFilter("CSSPreprocessor", ValidationComplete, CSSFilterFactory);
}

CSSPreprocessor::CSSPreprocessor(ConstPackagePtr pkg, CSSSubstitutionList substitutions) : ContentFilter(ShouldApply), m_substitutions(substitutions)
{
}


void* CSSPreprocessor::FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen)
{
    CSSFilterContext* p = dynamic_cast<CSSFilterContext*>(context);
    bool isCSS = p->isCSS();
    char* input = reinterpret_cast<char*>(data);
    
    std::string output;
    if (isCSS) {
        output.assign(input, len);
        for (CSSSubstitution& substitution: m_substitutions) {
            output = REGEX_NS::regex_replace(output, substitution.GetSearchRegex(), substitution.GetReplaceFormat());
        }
    }
    else
    {
        std::string toutput;
        // find each `style` tags
        REGEX_NS::cregex_iterator pos(input, input+len, StyleTagMatcher);
        REGEX_NS::cregex_iterator end;
        if ( pos == end )
        {
            toutput.assign(input, len);
        }
        else
        {
            while (pos != end) {
                //std::cout << "style tag prefix" << pos->prefix();
                toutput += pos->prefix();
                
                //for (int i = 0; i < pos->size(); i++) {
                //    std::cout << "style tag match " << i <<" "<< pos->length(i) <<" "<< pos->str(i) << "// ";
                //}
                
                //std::cout << "style match" << pos->str();
                std::string str = pos->str();
                for (CSSSubstitution& substitution: m_substitutions) {
                    str = REGEX_NS::regex_replace(str, substitution.GetSearchRegex(), substitution.GetReplaceFormat());
                }
                toutput += str;
                
                auto here = pos++;
                if ( pos == end )
                    toutput += here->suffix();
            }
        }
        
        // find each `style` attributes
        REGEX_NS::sregex_iterator apos(toutput.begin(), toutput.end(), StyleAttributeMatcher);
        REGEX_NS::sregex_iterator aend;
        if ( apos == aend )
        {
            output = toutput;        // no match == no change
        }
        else {
            while (apos != aend) {
                //std::cout << "style attr prefix" << apos->prefix();
                output += apos->prefix();
                
                //for (int i = 0; i < apos->size(); i++) {
                //    std::cout << "style attr match " << i <<" "<< apos->length(i) <<" "<< apos->str(i) << "// ";
                //}
                
                std::string str = apos->str();
                for (CSSSubstitution& substitution: m_substitutions) {
                    str = REGEX_NS::regex_replace(str, substitution.GetSearchRegex(), substitution.GetReplaceFormat());
                }
                output += str;
                
                auto here = apos++;
                if ( apos == aend )
                    output += here->suffix();
            }
        }
        
    }

    *outputLen = output.size();
    if ( output.size() < len )
    {
        // use the incoming buffer directly
        output.copy(input, output.size());
        return input;
    }
    
    // allocate an output buffer
    char* result = new char[output.size()];
    output.copy(result, output.size());
    return result;
}

EPUB3_END_NAMESPACE
