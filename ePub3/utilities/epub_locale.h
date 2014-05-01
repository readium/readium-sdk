//
//  epub_locale.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-07.
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

#ifndef __ePub3__locale__
#define __ePub3__locale__

#include <ePub3/utilities/basic.h>
#include <ePub3/utilities/utfstring.h>
#include <locale>

EPUB3_BEGIN_NAMESPACE

/**
 Returns the API-wide current locale.
 
 This value is initially set to the current user locale, but it can be
 explicitly changed (i.e. for testing purposes) by calling
 SetLocale(const string&) or SetLocale(const std::locale&).
 @return A reference to the current C++11 locale object.
 */
EPUB3_EXPORT
std::locale& CurrentLocale();

/**
 Sets the current locale to a given `std::locale` instance.
 @param locale The new locale.
 */
EPUB3_EXPORT
void SetCurrentLocale(const std::locale& newLocale);

/**
 Sets the current locale using a standard locale name.
 @param name A string containing a canonical locale name.
 @return A reference to the selected locale.
 */
EPUB3_EXPORT
std::locale& SetCurrentLocale(const string& name);

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__locale__) */
