//
//  nav_point.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__nav_point__
#define __ePub3__nav_point__

#include "epub3.h"
#include "nav_element.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class NavigationPoint : public NavigationElement
{
public:
    NavigationPoint() = delete;
    NavigationPoint(xmlNodePtr node); // NB: does NOT cascade to create children
    NavigationPoint(const std::string& ident, const std::string& label, const std::string& content) : NavigationElement(), _label(label), _content(content) {}
    NavigationPoint(const NavigationPoint&) = delete;
    NavigationPoint(NavigationPoint&& o) : NavigationElement(o), _label(std::move(o._label)), _content(std::move(o._content)) {}
    virtual ~NavigationPoint() {}
    
    virtual const std::string& Title() const { return _label; }
    virtual void SetTitle(const std::string& str) { _label = str; }
    virtual void SetTitle(std::string&& str) { _label = str; }
    
    const std::string& Content() const { return _content; }
    
protected:
    std::string _label;
    std::string _content;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_point__) */
