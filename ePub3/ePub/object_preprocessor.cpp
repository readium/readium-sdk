//
//  object_preprocessor.cpp
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

#include "object_preprocessor.h"
#include "package.h"
#include "filter_manager.h"

static const REGEX_NS::regex::flag_type regexFlags(REGEX_NS::regex::ECMAScript|REGEX_NS::regex::optimize);
static const REGEX_NS::regex reEscaper("\\\\\\.\\(\\)\\[\\]\\$\\^\\*\\+\\?\\:\\=\\|", regexFlags);

EPUB3_BEGIN_NAMESPACE

static REGEX_NS::regex ParamMatcher("<param[^>]+(name|value)=\"([^\"]*)\"[^>]*?(value|name)=\"([^\"]*)\"(.|\\n|\\r)*?>", regexFlags);
static REGEX_NS::regex SourceFinder("data=\\\"([^\\\"]*)\\\"", regexFlags);
static REGEX_NS::regex IDFinder("id=\\\"([^\\\"]*)\\\"", regexFlags);

bool ObjectPreprocessor::ShouldApply(ConstManifestItemPtr item)
{
    return (item->MediaType() == "application/xhtml+xml" || item->MediaType() == "text/html");
}
ContentFilterPtr ObjectPreprocessor::ObjectFilterFactory(ConstPackagePtr package)
{
    if ( package->MediaTypesWithDHTMLHandlers().empty() )
        return nullptr;
    return New(package, "Open");
}
void ObjectPreprocessor::Register()
{
    FilterManager::Instance()->RegisterFilter("ObjectPreprocessor", ObjectPreprocessing, ObjectFilterFactory);
}
ObjectPreprocessor::ObjectPreprocessor(ConstPackagePtr pkg, const string& buttonTitle) : ContentFilter(ShouldApply), _button(buttonTitle)
{
    Package::StringList mediaTypes = pkg->MediaTypesWithDHTMLHandlers();
    if ( mediaTypes.empty() )
    {
        // No work for the filter to do here -- disable all matches
        SetTypeSniffer([](ConstManifestItemPtr){return false;});
        return;
    }
    
    std::stringstream ss;
    ss << "<object\\s+?([^>]*?(?:media-)?type=\"(";
    
    auto pos = mediaTypes.begin();
    auto end = mediaTypes.end();
    
    while ( pos != end )
    {
        auto here = pos++;
        std::string str = REGEX_NS::regex_replace(here->stl_str(), reEscaper, "$`\\\\$&$'");    // yes, double-backslash, so it doesn't escape whatever '$&' is (i.e. the character which needs to be escaped)
        if ( pos == end )
            ss << str;
        else
            ss << str << "|";
    }
    
    ss << ")\"[^>]*?)>((?:.|\\n|\\r)*?)</object>";
    std::string reStr = ss.str();
    _objectMatcher = REGEX_NS::regex(reStr, regexFlags);
    
    for ( auto mediaType : mediaTypes )
    {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
        _handlers.emplace(mediaType, *(pkg->OPFHandlerForMediaType(mediaType)));
#else
        _handlers.insert({mediaType, *(pkg->OPFHandlerForMediaType(mediaType))});
#endif
    }
}
void* ObjectPreprocessor::FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen)
{
    char* input = reinterpret_cast<char*>(data);
    // find each `object` tag
    REGEX_NS::cregex_iterator pos(input, input+len, _objectMatcher);
    REGEX_NS::cregex_iterator end;
    if ( pos == end )
    {
        *outputLen = len;
        return data;        // no match == no change
    }
    
    std::string output;
    while ( pos != end )
    {
        // output any leading non-matched characters
        output += pos->prefix();
        
        std::string type(pos->str(2));
        // we have matched an <object> element: find the appropriate media handler
        auto found = _handlers.find(type);
        if ( found == _handlers.end() )
        {
            output += pos->str();
            continue;
        }
        
        const MediaHandler& handler = found->second;
        ContentHandler::ParameterList params;
        params["type"] = type;
        
        // find the data source
        REGEX_NS::smatch m;
        std::string attrs(pos->str(1));
        REGEX_NS::regex_search(attrs.cbegin(), attrs.cend(), m, SourceFinder);
        string src(m[1].str());
        
        // find any parameters to the object tag
        std::string content(pos->str(3));
        REGEX_NS::sregex_iterator cpos(content.begin(), content.end(), ParamMatcher);
        REGEX_NS::sregex_iterator cend;
        
        while ( cpos != cend )
        {
            if ( cpos->length() >= 4 )
            {
                string name, value;
                if ( cpos->str(1) == "name" )
                    name = cpos->str(2);
                else
                    value = cpos->str(2);
                
                if ( cpos->str(3) == "value" )
                    value = cpos->str(4);
                else
                    name = cpos->str(4);
                
                params[name] = value;
            }
            
            ++cpos;
        }
        
        // now determine the target-- this is an absolute URL
        IRI target = handler.Target(src, params);
        
        // find out if the object tag had an id attribute
        std::string objectID;
        if ( REGEX_NS::regex_search(attrs.cbegin(), attrs.cend(), m, IDFinder) )
            objectID = m[1].str();
        
        // now construct the `iframe` tag
        std::string url = target.URIString().stl_str();
        output += "<iframe src=\"" + url + "\" srcdoc=\"" + url + "\"";
        
        // replicate any id attribute from the `object` tag
        if ( !objectID.empty() )
            output += " id=\"" + objectID + "\"";
        
        // enable sandbox and allow some stuff, and use seamless presentation
        output += " sandbox=\"allow-forms allow-scripts allow-same-origin\" seamless=\"seamless\"></iframe>";
        
        // now add the form & button
        output += "<form action=\"" + url + "\" method=\"get\"";
        if ( !objectID.empty() )
            output += " id=\"" + objectID + "-form\"";
        output += "><button type=\"submit\"";
        if ( !objectID.empty() )
            output += " id=\"" + objectID + "-button\"";
        output += ">" + _button.stl_str() + "</button></form>";
        
        // that's it-- we've replaced the whole lot!
        
        // step forward, and if we get the end let's output the current suffix
        // this ensures that we don't lose everything following the last match
        auto here = pos++;
        if ( pos == end )
            output += here->suffix();
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
