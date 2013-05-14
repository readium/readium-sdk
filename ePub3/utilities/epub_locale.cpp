//
//  epub_locale.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-07.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
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
