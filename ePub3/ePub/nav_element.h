//
//  nav_element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-12.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef ePub3_nav_element_h
#define ePub3_nav_element_h

#include "epub3.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class NavigationElement;

typedef std::vector<NavigationElement*> NavigationList;

// abstract base for polymorphic navigation table/point classes
class NavigationElement
{
public:
    explicit NavigationElement() = default;
    explicit NavigationElement(const NavigationElement&) = default;
    explicit NavigationElement(NavigationElement&& o) : _children(std::move(o._children)) {}
    virtual ~NavigationElement() { for ( auto __p : _children ) { delete __p; } }
    
    virtual const std::string& Title() const = 0;
    virtual void SetTitle(const std::string& str) = 0;
    virtual void SetTitle(std::string&& str) = 0;
    
    const NavigationList& Children() const { return _children; }
    
    void AppendChild(NavigationElement* e) { _children.push_back(e); }
    
protected:
    NavigationList  _children;
};

EPUB3_END_NAMESPACE

#endif
