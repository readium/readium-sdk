//
//  media_support_info.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
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

#ifndef __ePub3__media_support_info__
#define __ePub3__media_support_info__

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>
#include <map>
#include <list>

EPUB3_BEGIN_NAMESPACE

class Package;
class ManifestItem;

/**
 The MediaSupportInfo class provides at-a-glance information to the EPUB engine describing
 whether a given manifest item can be handled by the reading system.
 
 A media type can be intrinsically supported as an EPUB 3 core media type or
 a foreign media type which is supported by the rendering engine. Alternatively a
 type might be supported through a DHTML handler defined through the `bindings`
 element from an OPF file. Any other types are considered unsupported, thus triggering
 selection of any fallback manifest item in its stead.
 
 @ingroup utilities
 */
class MediaSupportInfo : public PointerType<MediaSupportInfo>, public OwnedBy<Package>
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    ///
    /// A list of manifest items.
    typedef shared_vector<ManifestItem>   ManifestItemList;
    
    ///
    /// Possible types of support provided.
    enum class EPUB3_EXPORT SupportType : uint8_t
    {
        Unsupported             = 0,    ///< The media type is not supported.
        IntrinsicSupport        = 1,    ///< The rendering engine understands this media type with no outside help.
        SupportedWithHandler    = 2     ///< The publication provides a DHTML handler for this media type.
    };
    
public:
    ///
    /// The default constructor creates an object with no information.
    EPUB3_EXPORT    MediaSupportInfo(shared_ptr<Package> owner);
    ///
    /// A simple constructor, normally used to define intrinsic support.
    EPUB3_EXPORT    MediaSupportInfo(shared_ptr<Package> owner, const string& mediaType, bool supported=true);
    ///
    /// The real constructor takes a specific SupportType value.
    EPUB3_EXPORT    MediaSupportInfo(shared_ptr<Package> owner, const string& mediaType, SupportType support);
    ///
    /// Copy constructor.
    EPUB3_EXPORT    MediaSupportInfo(const MediaSupportInfo& o);
    ///
    /// C++11 'move' constructor.
    EPUB3_EXPORT    MediaSupportInfo(MediaSupportInfo&& o);
    ///
    /// Destructor.
    virtual         ~MediaSupportInfo() {}
    
    ///
    /// Copy assignment operator.
    EPUB3_EXPORT
    MediaSupportInfo&   operator=(const MediaSupportInfo& o);
    ///
    /// C++11 move assignment operator.
    EPUB3_EXPORT
    MediaSupportInfo&   operator=(MediaSupportInfo&& o);
    
    /// @{
    /// @name Accessors
    
    /**
     Returns the media type represented by this object.
     */
    const string    MediaType()             const   { return _mediaType; }
    
    /**
     Returns the type of support as a SupportType code.
     */
    SupportType     Support()               const   { return _support; }
    
    /**
     Queries whether a type is supported intrinsically by the rendering engine.
     */
    virtual bool    HasIntrinsicSupport()   const;
    
    /**
     Queries whether a type is supported through the use of a special handler.
     */
    virtual bool    RequiresMediaHandler()  const;
    
    /// @}
    
    /// @{
    /// @name Setters
    
    /**
     Sets the media type and its support state.
     
     This is here as a convenience to anyone working with C++ containers.
     @param mediaType The media type.
     @param support The type of support provided.
     */
    virtual void    SetTypeAndSupport(const string& mediaType, SupportType support);
    
    /// @}
    
    /**
     Constructs a list of manifest items with this media type.
     @param pkg The package whose manifest to look through.
     @result A vector containing manifest items matching the receiver's media type.
     */
    EPUB3_EXPORT
    const ManifestItemList  MatchingManifestItems(shared_ptr<Package> pkg) const;
    
protected:
    string              _mediaType;     ///< The media type to which this object refers.
    SupportType         _support;       ///< The type of support available for this type.
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__media_support_info__) */
