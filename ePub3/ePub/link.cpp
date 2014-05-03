//
//  link.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/28/2013.
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

#include "link.h"
#include "epub_collection.h"
#include "package.h"
#include "manifest.h"
#include <ePub3/utilities/error_handler.h>

EPUB3_BEGIN_NAMESPACE

static void RemoveFragment(string_view& str)
{
    auto idx = str.rfind('#');
    if (idx != string_view::npos)
        str.remove_suffix(str.size()-idx);
}
static bool MatchesWithoutFragment(string_view a, string_view b)
{
    RemoveFragment(a);
    RemoveFragment(b);
    return a == b;
}

Link::Link(CollectionPtr owner)
    : PointerType(), OwnedBy(owner), _href(), _rel()
#if EPUB_PLATFORM(WINRT)
    , NativeBridge()
#endif
{
}
#if !EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
Link::Link(const Link& o)
    : PointerType(o), OwnedBy(o), _href(o._href), _rel(o._rel)
#if EPUB_PLATFORM(WINRT)
    , NativeBridge(o)
#endif
{
}
Link::Link(Link&& o)
    : PointerType(std::move(o)), OwnedBy(std::move(o)), _href(std::move(o._href)), _rel(std::move(o._rel))
#if EPUB_PLATFORM(WINRT)
    , NativeBridge(std::move(o))
#endif
{
}
Link& Link::operator=(const Link& o)
{
    OwnedBy::operator=(o);
#if EPUB_PLATFORM(WINRT)
    NativeBridge::operator=(o);
#endif
    _href = o._href;
    _rel = o._rel;
    return *this;
}
Link& Link::operator=(Link&& o)
{
    OwnedBy::operator=(std::move(o));
#if EPUB_PLATFORM(WINRT)
    NativeBridge::operator=(std::move(o));
#endif
    _href = std::move(o._href);
    _rel = std::move(o._rel);
    return *this;
}
#endif
bool Link::ParseXML(shared_ptr<xml::Node> node)
{
    // should I explicitly put these in the OPF namespace?
    _href = _getProp(node, "href");
    _rel = _getProp(node, "rel");
    _type = _getProp(node, "media-type");
    
    if (_href.empty())
        HandleError(EPUBError::OPFLinkMissingHref, "No href attribute in <collection> <link> element.");
    
    return true;
}
ConstManifestItemPtr Link::ReferencedManifestItem() const
{
    ConstPackagePtr pkg = Owner()->GetPackage();
    if (!bool(pkg))
        return nullptr;
    
    for (auto& item : pkg->Manifest())
    {
        // remember: <collection> is within the same OPF file as the manifest, so
        //  relative paths will be the same.
        if (MatchesWithoutFragment(item.second->Href(), _href))
            return item.second;
    }
    
    return nullptr;
}

EPUB3_END_NAMESPACE
