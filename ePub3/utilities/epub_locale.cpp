//
//  epub_locale.cpp
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

#include <ePub3/utilities/epub_locale.h>

EPUB3_BEGIN_NAMESPACE

// empty string means 'current user locale', no parameter/nullptr means 'C locale'
static std::locale  gCurrentLocale("");

EPUB3_EXPORT
std::locale& CurrentLocale()
{
    return gCurrentLocale;
}

EPUB3_EXPORT
void SetCurrentLocale(const std::locale& newLocale)
{
    gCurrentLocale = newLocale;
}

EPUB3_EXPORT
std::locale& SetCurrentLocale(const string& name)
{
#if EPUB_OS(ANDROID)
    gCurrentLocale = std::locale(name.c_str());
#else
    gCurrentLocale = std::locale(name.stl_str());
#endif
    return gCurrentLocale;
}

EPUB3_END_NAMESPACE
