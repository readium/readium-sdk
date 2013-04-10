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

#include "_platform.h"

#if EPUB_OS(WINDOWS)
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
