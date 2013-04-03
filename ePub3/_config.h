//
//  _config.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
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

////////////////////////////////////////////////////////////////////////////////
// General doxygen input for the project as a whole

/**
 @mainpage
 @copyright Copyright (c) 2012–2013 The Readium Foundation and contributors. Released under the GNU Public License, version 3.
 @author Jim Dovey
 @author Boris Schneidermann
 @author Shane Meyer
 */

// I'm putting all Doxygen group definitions in here

/**
 @defgroup utilities Utilities
 
 @defgroup archives Archive Handling
 
 @defgroup epub EPUB® 3 Engine
 @{
    @defgroup epub-model EPUB® 3 Model
    @{
        @defgroup navigation Navigation
    @}

    @defgroup content-proc EPUB® 3 Content Processing
    @{
        @defgroup filters Content Filters
        @defgroup media-handlers Content Handlers
        @defgroup security Encryption and Signatures
    @}
    
    @defgroup events Events Mechanism
 @}
 
 @defgroup xml XML Engine
 @{
    @defgroup validation Content Validation
    @defgroup parsing XML Parsers
    @defgroup tree DOM Tree
    @defgroup xml-utils Utilities
    @defgroup xmlsec XML Security
 @}
 */

// end of global Doxygen input
////////////////////////////////////////////////////////////////////////////////

#ifndef ePub3__config_h
#define ePub3__config_h

#if (defined(__CYGWIN32__) || defined(_WIN32)) && !defined(__WIN32__)
#define __WIN32__ 1
#endif

#if defined(_WIN64) && !defined(__WIN64__)
#define __WIN64__ 1
#endif

#if defined(__WIN64__) && !defined(__LLP64__)
#define __LLP64__ 1
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
#define __i386__ 1
#endif

#if defined(__WIN32__) || defined(__WIN64__)
#define EPUB3_TARGET_WINDOWS 1
#endif

#if (defined(__i386__) || defined(__x86_64__)) && !defined(__LITTLE_ENDIAN__)
#define __LITTLE_ENDIAN__ 1
#endif

#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#error Do not know the endianess of this architecture
#endif

#if !__BIG_ENDIAN__ && !__LITTLE_ENDIAN__
#error Both __BIG_ENDIAN__ and __LITTLE_ENDIAN__ cannot be false
#endif

#if __BIG_ENDIAN__ && __LITTLE_ENDIAN__
#error Both __BIG_ENDIAN__ and __LITTLE_ENDIAN__ cannot be true
#endif

// Some compilers provide the capability to test if certain features are available. This macro provides a compatibility path for other compilers.
#ifndef __has_feature
#define __has_feature(x) 0
#endif

// Some compilers provide the capability to test if certain attributes are available. This macro provides a compatibility path for other compilers.
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_extension
#define __has_extension(x) 0
#endif

#if EPUB3_TARGET_WINDOWS
# ifndef EPUB3_EXPORT
#  ifdef BUILDING_EPUB3
#   define EPUB3_EXPORT __declspec(dllexport)
#  else
#   define EPUB3_EXPORT __declspec(dllimport)
#  endif
# endif
#else
# define EPUB3_EXPORT
#endif

#endif
