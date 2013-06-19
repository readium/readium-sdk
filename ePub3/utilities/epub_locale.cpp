//
//  epub_locale.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-07.
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
