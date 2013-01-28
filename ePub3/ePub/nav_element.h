//
//  nav_element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-12.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#ifndef ePub3_nav_element_h
#define ePub3_nav_element_h

#include "epub3.h"
#include "utfstring.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class NavigationElement;

typedef std::vector<NavigationElement*> NavigationList;

// abstract base for polymorphic navigation table/point classes
class NavigationElement
{
public:
    explicit            NavigationElement()                             = default;
    explicit            NavigationElement(const NavigationElement&)     = default;
    explicit            NavigationElement(NavigationElement&& o) : _children(std::move(o._children)) {}
    virtual             ~NavigationElement() {}
    
    virtual const string&   Title()                     const   = 0;
    virtual void            SetTitle(const string& str)         = 0;
    virtual void            SetTitle(string&& str)              = 0;
    
    const NavigationList&   Children()                  const   { return _children; }
    
    void                    AppendChild(NavigationElement* e)   { _children.push_back(e); }
    
protected:
    NavigationList  _children;
};

EPUB3_END_NAMESPACE

#endif
