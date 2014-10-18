//
//  nav_element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-12.
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

#ifndef ePub3_nav_element_h
#define ePub3_nav_element_h

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/owned_by.h>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class NavigationElement;

typedef shared_ptr<NavigationElement> NavigationElementPtr;
typedef shared_vector<NavigationElement>    NavigationList;

// abstract base for polymorphic navigation table/point classes
/**
 @ingroup navigation
 */
class NavigationElement
{
public:
    explicit                NavigationElement() : _children() {}
    explicit                NavigationElement(const NavigationElement& o) : _children(o._children) {}
    explicit                NavigationElement(NavigationElement&& o) : _children(std::move(o._children)) {}
    virtual                 ~NavigationElement() {}
    
    virtual const string&   Title()                     const   = 0;
    virtual void            SetTitle(const string& str)         = 0;
    virtual void            SetTitle(string&& str)              = 0;
    
    const NavigationList&   Children()                  const   { return _children; }
    
    void                    AppendChild(shared_ptr<NavigationElement> e)   { _children.push_back(e); }
    
protected:
    NavigationList          _children;
};

EPUB3_END_NAMESPACE

#endif
