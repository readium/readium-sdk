//
//  object_preprocessor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#include "object_preprocessor.h"
#include "package.h"

static const std::regex reEscaper(R"X(\\\.\(\)\[\]\$\^\*\+\?\:\=\|)X", std::regex::ECMAScript|std::regex::optimize);

EPUB3_BEGIN_NAMESPACE

static std::regex ParamMatcher(R"X(<param[^>]+(name|value)="([^"]*)"[^>]*?(value|name)="([^"]*)"(.|\n|\r)*?>)X", std::regex::ECMAScript|std::regex::optimize|std::regex::icase);
static std::regex SourceFinder("data=\"([^\"]*)\"", std::regex::ECMAScript|std::regex::optimize|std::regex::icase);
static std::regex IDFinder("id=\"([^\"]*)\"", std::regex::ECMAScript|std::regex::optimize|std::regex::icase);

bool ObjectPreprocessor::ShouldApply(const ePub3::ManifestItem *item, const ePub3::EncryptionInfo *encInfo __unused)
{
    return (item->MediaType() == "application/xhtml+xml" || item->MediaType() == "text/html");
}
ObjectPreprocessor::ObjectPreprocessor(const Package* pkg, const string& buttonTitle) : ContentFilter(ShouldApply), _button(buttonTitle)
{
    Package::StringList mediaTypes = pkg->MediaTypesWithDHTMLHandlers();
    if ( mediaTypes.empty() )
    {
        // No work for the filter to do here -- disable all matches
        SetTypeSniffer([](const ManifestItem* __unused, const EncryptionInfo* __unused){return false;});
        return;
    }
    
    std::stringstream ss;
    ss << R"X(<object\s+?([^>]*?(?:media-)?type="()X";
    
    auto pos = mediaTypes.begin();
    auto end = mediaTypes.end();
    
    while ( pos != end )
    {
        auto here = pos++;
        std::string str = std::regex_replace(here->stl_str(), reEscaper, R"X($`\\$&$')X");    // yes, double-backslash, so it doesn't escape whatever '$&' is (i.e. the character which needs to be escaped)
        if ( pos == end )
            ss << str;
        else
            ss << str << "|";
    }
    
    ss << R"X()"[^>]*?)>((?:.|\n|\r)*?)</object>)X";
    std::string reStr = ss.str();
    _objectMatcher = std::regex(reStr, std::regex::icase|std::regex::optimize|std::regex::ECMAScript);
    
    for ( auto mediaType : mediaTypes )
    {
        _handlers.emplace(mediaType, *(pkg->OPFHandlerForMediaType(mediaType)));
    }
}
void* ObjectPreprocessor::FilterData(void *data, size_t len, size_t *outputLen)
{
    char* input = reinterpret_cast<char*>(data);
    // find each `object` tag
    std::cregex_iterator pos(input, input+len, _objectMatcher);
    std::cregex_iterator end;
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
        ContentHandler::ParameterList params({{"type", type}});
        
        // find the data source
        std::smatch m;
        std::string attrs(pos->str(1));
        std::regex_search(attrs.cbegin(), attrs.cend(), m, SourceFinder);
        string src(m[1].str());
        
        // find any parameters to the object tag
        std::string content(pos->str(3));
        std::sregex_iterator cpos(content.begin(), content.end(), ParamMatcher);
        std::sregex_iterator cend;
        
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
        if ( std::regex_search(attrs.cbegin(), attrs.cend(), m, IDFinder) )
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
