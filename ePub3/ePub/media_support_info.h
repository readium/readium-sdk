//
//  media_support_info.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-04.
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
