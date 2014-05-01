//
//  content_handler.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-30.
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

#ifndef __ePub3__content_handler__
#define __ePub3__content_handler__

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/utilities/owned_by.h>
#include <map>
#include <functional>

EPUB3_BEGIN_NAMESPACE

class Package;

/**
 The ContentHandler class defines an interface by which resources of a particular
 media type can be handled.
 
 Concrete subclasses exist which make use of a media handler defined in the OPF
 `<bindings>` element and for a custom renderer which takes a source path and a
 Package reference. The most common use is for the former case, when a resource with
 a foreign media-type is opened as a top-level document: the handler will instead
 open the DHTML handler document and pass that the source path as a URL query
 parameter.
 
 A ContentHandler is initialized with a media-type string (similar to a MIME type)
 and a reference to the package to which this handler is assigned. It operates as a
 functor and is thus compatible with a C++11 `std::function`. Its 'call' operator
 takes two parameters, a Package-relative path and a list of key/value pairs
 defining any parameters (i.e. for a URL query string).
 
 @remarks The ContentHandler class keeps a non-owning reference to a Package
 instance.
 
 @ingroup media-handlers
 @see operator()(const string&, const ParameterList&)
 */
class ContentHandler : public PointerType<ContentHandler>, public OwnedBy<Package>
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    ///
    /// A list of key/value pairs a resource's invocation parameters.
    typedef std::map<string, string>        ParameterList;

private:
    ///
    /// No default constructor.
                            ContentHandler() _DELETED_;
    
public:
    /**
     Creates a content handler (abstract class).
     @param mediaType The media type to which this handler should apply.
     @param pkg The Package to which this handler is assigned.
     */
                            ContentHandler(shared_ptr<Package>& owner, const string& mediaType) : OwnedBy(owner), _mediaType(mediaType) {}
    ///
    /// Copy constructor.
                            ContentHandler(const ContentHandler& o) : OwnedBy(o), _mediaType(o._mediaType) {}
    ///
    /// Move constructor.
    ContentHandler(ContentHandler&& o) : OwnedBy(std::move(o)), _mediaType(std::move(o._mediaType)) {}
    virtual                 ~ContentHandler() {}
    
    virtual ContentHandler& operator=(const ContentHandler& o) {
        _mediaType = o._mediaType;
		OwnedBy::operator=(o);
        return *this;
    }
    virtual ContentHandler& operator=(ContentHandler&& o) {
        _mediaType = std::move(o._mediaType);
		OwnedBy::operator=(std::move(o));
        return *this;
    }
    
    ///
    /// Obtains the media-type this object handles.
    virtual const string&   MediaType()     const   { return _mediaType; }
    /**
     Invokes the handler.
     @param src The Package-relative path to a resource of this handler's media-type.
     @param parameters A list of key/value pairs defining parameters to be used when
     handling the resource.
     */
    virtual void            operator()(const string& src,
                                       const ParameterList& parameters = ParameterList())   const   = 0;
    
protected:
    string                  _mediaType;     ///< The resource media-type that this object handles.
};

/**
 A ContentHandler subclass which uses an `OPF` DHTML foreign media handler to display
 resources.
 
 @ingroup media-handlers
 */
class MediaHandler : public ContentHandler
{
private:
    ///
    /// No default constructor.
    MediaHandler() _DELETED_;
    
public:
    /**
     Creates a media handler.
     @param pkg The Package to which this handler is assigned.
     @param mediaType The media-type to which this handler should apply.
     @param handlerPath A Package-relative path to the DHTML media handler for
     `mediaType` resources.
     */
                        MediaHandler(shared_ptr<Package>& owner, const string& mediaType, const string& handlerPath);
    ///
    /// Copy constructor.
                        MediaHandler(const MediaHandler& o) : ContentHandler(o), _handlerIRI(o._handlerIRI) {}
    ///
    /// Move constructor.
                        MediaHandler(MediaHandler&& o) : ContentHandler(std::move(o)), _handlerIRI(std::move(o._handlerIRI)) {}
    virtual             ~MediaHandler() {}
    
    /**
     Invokes the media handler with a given resource and parameters.
     
     This class's implementation constructs a URL to the DHTML media handler and
     adds `src` as a URL query parameter along with any additional parameters
     provided.
     @param src The Package-relative path to a resource to display.
     @param parameters A set of key/value pairs defining additional parameters to the
     media handler.
     */
    virtual void        operator()(const string& src, const ParameterList& parameters = ParameterList())    const;
    
    /**
     Returns a URL to load the DHTML media handler for a resource.
     @param src The Package-relative path to a resource to display.
     @param parameters URL query parameters to the DHTML media handler.
     */
    virtual IRI         Target(const string& src, const ParameterList& parameters)                          const;
    
protected:
    const IRI           _handlerIRI;        ///< The URL of a DHTML media handler.
};

/**
 Implements a means by which a native renderer for a foreign media-type can be invoked.
 
 This class is initialized with a callback function; whenever a matching resource is
 to be loaded, this callback will be invoked, passing in the Package-relative
 resource path and a reference to its containing Package.
 
 @ingroup media-handlers
 */
class CustomRenderer : public ContentHandler
{
public:
    /**
     A custom resource renderer callback.
     @param src The Package-relative path to a resource.
     @param pkg The Package containing the resource.
     */
    typedef std::function<void(const string& src, ConstPackagePtr pkg)>  RendererImpl;

private:
    ///
    /// No default constructor.
    CustomRenderer() _DELETED_;
    
public:
    /**
     Creates a new custom renderer.
     @param mediaType The media-type that this object handles.
     @param pkg The Package to which this handler is assigned.
     @param impl A callback function to the native renderer.
     */
                        CustomRenderer(shared_ptr<Package>& owner, const string& mediaType, RendererImpl impl) : ContentHandler(owner, mediaType), _impl(impl) {}
    ///
    /// Copy constructor.
                        CustomRenderer(const CustomRenderer& o) : ContentHandler(o), _impl(o._impl) {}
    ///
    /// Move constructor.
                        CustomRenderer(CustomRenderer&& o) : ContentHandler(std::move(o)), _impl(std::move(o._impl)) {}
    virtual             ~CustomRenderer() {}
    
    /**
     Invokes the custom renderer.
     
     This implementation simply invokes the callback function provided when this
     object was created, passing it the input resource path and a reference to the
     owning Package instance. It does not use the `parameters` argument at all.
     @param src The Package-relative path to a resource to display.
     @param parameters Parameters to the renderer. Unused.
     */
    virtual void        operator()(const string& src, const ParameterList& parameters = ParameterList())    const { _impl(src, Owner()); }
    
protected:
    RendererImpl        _impl;      ///< The native renderer callback.
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__content_handler__) */
