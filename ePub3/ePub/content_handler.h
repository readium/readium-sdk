//
//  content_handler.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-30.
//  Copyright (c) 2013 The Readium Foundation.
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

#ifndef __ePub3__content_handler__
#define __ePub3__content_handler__

#include "epub3.h"
#include "utfstring.h"
#include "iri.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

class Package;

class ContentHandler
{
public:
    typedef std::map<string, string>        ParameterList;
    
public:
                            ContentHandler(const string& mediaType, const Package* pkg=nullptr) : _mediaType(mediaType), _owner(pkg) {}
                            ContentHandler() = delete;
                            ContentHandler(const ContentHandler& o) : _mediaType(o._mediaType), _owner(o._owner) {}
                            ContentHandler(ContentHandler&& o) : _mediaType(std::move(o._mediaType)), _owner(o._owner) { o._owner = nullptr; }
    virtual                 ~ContentHandler() {}
    
    virtual const Package*  Owner()         const   { return _owner; }
    virtual const string&   MediaType()     const   { return _mediaType; }
    virtual void            operator()(const string& src,
                                       const ParameterList& parameters = ParameterList())   const   = 0;
    
protected:
    const Package*          _owner;
    const string            _mediaType;
};

class MediaHandler : public ContentHandler
{
public:
                        MediaHandler(const Package* pkg, const string& mediaType, const string& handlerPath) : ContentHandler(mediaType, pkg), _handlerPath(handlerPath) {}
                        MediaHandler() = delete;
                        MediaHandler(const MediaHandler& o) : ContentHandler(o), _handlerPath(o._handlerPath) {}
                        MediaHandler(MediaHandler&& o) : ContentHandler(std::move(o)), _handlerPath(std::move(o._handlerPath)) {}
    virtual             ~MediaHandler() {}
    
    virtual void        operator()(const string& src, const ParameterList& parameters = ParameterList())    const;
    virtual IRI         Target(const string& src, const ParameterList& parameters)                          const;
    
protected:
    const string        _handlerPath;
};

class CustomRenderer : public ContentHandler
{
public:
    typedef std::function<void(const string& src, const Package* pkg)>  RendererImpl;
    
public:
                        CustomRenderer(const string& mediaType, const Package* pkg, RendererImpl impl) : ContentHandler(mediaType, pkg), _impl(impl) {}
                        CustomRenderer() = delete;
                        CustomRenderer(const CustomRenderer& o) : ContentHandler(o), _impl(o._impl) {}
                        CustomRenderer(CustomRenderer&& o) : ContentHandler(std::move(o)), _impl(std::move(o._impl)) {}
    virtual             ~CustomRenderer() {}
    
    virtual void        operator()(const string& src, const ParameterList& parameters = ParameterList())    const { _impl(src, Owner()); }
    
protected:
    RendererImpl        _impl;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__content_handler__) */
