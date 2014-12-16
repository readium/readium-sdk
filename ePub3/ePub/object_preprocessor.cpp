//
//  object_preprocessor.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
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

#include "object_preprocessor.h"
#include "package.h"
#include "filter_manager.h"
#include "iri.h"
#include <google-url/gurl.h>

static const REGEX_NS::regex::flag_type regexFlags(REGEX_NS::regex::ECMAScript|REGEX_NS::regex::optimize);
static const REGEX_NS::regex reEscaper("\\\\\\.\\(\\)\\[\\]\\$\\^\\*\\+\\?\\:\\=\\|", regexFlags);

EPUB3_BEGIN_NAMESPACE

static REGEX_NS::regex ParamMatcher("<param[^>]+(name|value)=\"([^\"]*)\"[^>]*?(value|name)=\"([^\"]*)\"(.|\\n|\\r)*?>", regexFlags);
static REGEX_NS::regex SourceFinder("data=\\\"([^\\\"]*)\\\"", regexFlags);
static REGEX_NS::regex IDFinder("id=\\\"([^\\\"]*)\\\"", regexFlags);

FilterContext *ObjectPreprocessor::InnerMakeFilterContext(ConstManifestItemPtr item) const
{
    return new ObjectPreprocessorContext(item);
}
bool ObjectPreprocessor::ShouldApply(ConstManifestItemPtr item)
{
    if (item->MediaType() == "application/xhtml+xml" || item->MediaType() == "text/html")
    {
        // DEBUG!!
        IRI iri1 = IRI("http://github.com/danielweck#NO-BINDINGS");
        if (item->ContainsProperty(iri1, false))
        {
            PropertyPtr prop1 = item->PropertyMatching(iri1, false);
            if (prop1->Value() == "TRUE")
            {
                string href = item->Href();
                printf("SKIP BINDINGS: %s\n", href.c_str());
                return false;
            }
        }

        return true;
    }

    return false;
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
        std::string regstr("$`\\\\$&$'");
        std::string str = REGEX_NS::regex_replace(here->stl_str(), reEscaper, regstr);    // yes, double-backslash, so it doesn't escape whatever '$&' is (i.e. the character which needs to be escaped)
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
    ObjectPreprocessorContext *ptContext = dynamic_cast<ObjectPreprocessorContext *>(context);
    if (ptContext == nullptr)
    {
        return nullptr;
    }

    char* input = reinterpret_cast<char*>(data);
    //printf("\n\n%s\n\n", input);

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

        // e.g. data.xml (path relative to the original XHTML spine item, not to the (XHTML) handler!)
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

        ConstManifestItemPtr manifestItem = ptContext->ManifestItem();
        string manifestItemAbsolutePath = manifestItem->AbsolutePath();
        if (manifestItemAbsolutePath.at(0) == '/')
        {
            manifestItemAbsolutePath = manifestItemAbsolutePath.substr(1, manifestItemAbsolutePath.length()-1);
        }
        string::size_type i = manifestItemAbsolutePath.find_last_of('/');
        string parentFolderPath = "/";
        if (i != string::npos && i > 0)
        {
            parentFolderPath = _Str(manifestItemAbsolutePath.substr(0, i), '/');
        }
        if (parentFolderPath.at(0) != '/')
        {
            parentFolderPath.insert(0, "/");
        }
        string srcAbsolutePath = _Str(parentFolderPath, src);

        std::string::size_type j = 0;
        while ((j = srcAbsolutePath.find("/./")) != string::npos)
        {
            //srcAbsolutePath.replace(j+1, 2, "");
            srcAbsolutePath.erase(j+1, 2);
        }
        if (srcAbsolutePath.find("./") == 0)
        {
            srcAbsolutePath = srcAbsolutePath.length() > 2 ? srcAbsolutePath.substr(2) : "";
        }

        j = 0;
        while ((j = srcAbsolutePath.find("../")) != string::npos)
        {
            std::string::size_type k = j < 2 ? 0 : srcAbsolutePath.rfind('/', j - 2);
            //std::string::size_type k = srcAbsolutePath.find_first_of('/', j);
            if (k == string::npos || k == 0)
            {
                //srcAbsolutePath.replace(0, (j + 2) + 1, "");
                srcAbsolutePath.erase(0, (j + 2) + 1);
            }
            else
            {
                //srcAbsolutePath.replace(k + 1, (j + 2) - k, "");
                srcAbsolutePath.erase(k + 1, (j + 2) - k);
            }
        }

        if (srcAbsolutePath.at(0) != '/')
        {
            srcAbsolutePath.insert(0, "/");
        }

        IRI handlerIRI = handler.HandlerIRI();
        string handlerAbsolutePath = handlerIRI.Path(true);
        //IRI handlerIRI(handlerIRI_.Scheme(), handlerIRI_.Host(), handlerAbsolutePath);
        //string check = handlerIRI.URIString().stl_str();

//        GURL* gurl = handlerIRI.UnderlyingURL();
//        GURL resolved = gurl->Resolve(srcAbsolutePath);
//        string relativePath = resolved.path(); // already percent-escaped (IRI::URLEncodeComponent())


        static REGEX_NS::regex _PathSplitter("/");
        std::vector<string> handlerAbsolutePath_chunks = handlerAbsolutePath.split(_PathSplitter);
        std::vector<string> srcAbsolutePath_chunks = srcAbsolutePath.split(_PathSplitter);

        auto handlerAbsolutePath_begin = handlerAbsolutePath_chunks.begin();
        auto srcAbsolutePath_begin = srcAbsolutePath_chunks.begin();

        auto handlerAbsolutePath_end = handlerAbsolutePath_chunks.end();
        auto srcAbsolutePath_end = srcAbsolutePath_chunks.end();

        auto handlerAbsolutePath_iterator = handlerAbsolutePath_begin;
        auto srcAbsolutePath_iterator = srcAbsolutePath_begin;

        while ( handlerAbsolutePath_iterator != handlerAbsolutePath_end
                && srcAbsolutePath_iterator != srcAbsolutePath_end )
        {
            if (*handlerAbsolutePath_iterator != *srcAbsolutePath_iterator)
            {
                break;
            }

            ++handlerAbsolutePath_iterator;
            ++srcAbsolutePath_iterator;
        }

        handlerAbsolutePath_end--;

        string relativePath;

        for (; handlerAbsolutePath_iterator != handlerAbsolutePath_end; ++handlerAbsolutePath_iterator) {
            relativePath += "/";
            relativePath += "..";
        }
        for (; srcAbsolutePath_iterator != srcAbsolutePath_end; ++srcAbsolutePath_iterator)
        {
            relativePath += "/";
            relativePath += *srcAbsolutePath_iterator;
        }

        if (relativePath.at(0) == '/')
        {
            relativePath.erase(0, 1);
        }

//        string packageBasePath = manifestItem->Owner()->BasePath();
//        if (packageBasePath.at(0) != '/')
//        {
//            packageBasePath.insert(0, "/");
//        }
//
//        std::string::size_type k = relativePath.find(packageBasePath.stl_str());
//        if (k == 0)
//        {
//            relativePath = relativePath.substr(packageBasePath.length());
//        }


        // now determine the target-- this is an absolute URL
        IRI target = handler.Target(relativePath, params);

        // find out if the object tag had an id attribute
        std::string objectID;
        if ( REGEX_NS::regex_search(attrs.cbegin(), attrs.cend(), m, IDFinder) )
            objectID = m[1].str();
        
        // now construct the `iframe` tag
        std::string url = target.URIString().stl_str();

//        std::string url = target.Path(true).stl_str();
//        if (url.at(0) == '/')
//        {
//            url.erase(0, 1);
//        }

        //printf("BINDING IFRAME URL:\n%s\n", url.c_str());

        // #include <algorithm>
        //std::replace(url.begin(), url.end(), "&", "&amp;");

        std::string buffer;
        buffer.reserve(url.size());
        for(size_t pos = 0; pos != url.size(); ++pos) {
            switch(url[pos]) {
                case '&':  buffer.append("&amp;");       break;
                case '\"': buffer.append("&quot;");      break;
                case '\'': buffer.append("&apos;");      break;
                case '<':  buffer.append("&lt;");        break;
                case '>':  buffer.append("&gt;");        break;
                default:   buffer.append(&url[pos], 1); break;
            }
        }
        url.swap(buffer);

        output += "<iframe src=\"" + url + "\"";
        //output += " srcdoc=\"" + url + "\"";
        
        // replicate any id attribute from the `object` tag
        if ( !objectID.empty() )
            output += " id=\"" + objectID + "\"";
        
        // enable sandbox and allow some stuff, and use seamless presentation
        output += " sandbox=\"allow-forms allow-scripts allow-same-origin\" seamless=\"seamless\"></iframe>";

//        // now add the form & button
//        output += "<form action=\"" + url + "\" method=\"get\"";
//        if ( !objectID.empty() )
//            output += " id=\"" + objectID + "-form\"";
//        output += "><button type=\"submit\"";
//        if ( !objectID.empty() )
//            output += " id=\"" + objectID + "-button\"";
//        output += ">" + _button.stl_str() + "</button></form>";
        
        // that's it-- we've replaced the whole lot!
        
        // step forward, and if we get the end let's output the current suffix
        // this ensures that we don't lose everything following the last match
        auto here = pos++;
        if ( pos == end )
            output += here->suffix();
    }
    
    *outputLen = output.size();

    if (*outputLen == 0)
    {
        *outputLen = len;
        return input;
    }

// DEBUG: to test large buffer addition to original resource size
//    output += "\n<!--\n";
//    for (int i = 0; i < 500; i++)
//    {
//        output += "abcdefghijklmnopqrstuvwxyz-0123456789_";
//    }
//    output += "\n-->\n";
//    *outputLen = output.size();

    if (*outputLen < len)
    {
        output.copy(input, *outputLen);
        return input;
    }

    char* result = new char[*outputLen];
    output.copy(result, *outputLen);
    return result;
}

EPUB3_END_NAMESPACE
