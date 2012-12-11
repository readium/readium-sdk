//
//  epub3.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-27.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef ePub3_epub3_h
#define ePub3_epub3_h

#define EPUB3_BEGIN_NAMESPACE namespace ePub3 {
#define EPUB3_END_NAMESPACE };

// this chunk will go away once we have the C++ XML interface complete
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

static inline std::string _getProp(xmlNodePtr node, const char *name)
{
    const xmlChar * ch = xmlGetProp(node, reinterpret_cast<const xmlChar*>(name));
    if ( ch == nullptr )
        return "";
    return reinterpret_cast<const char*>(ch);
}

EPUB3_END_NAMESPACE

#endif
