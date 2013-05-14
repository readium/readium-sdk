//
//  epub_locale.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-07.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
