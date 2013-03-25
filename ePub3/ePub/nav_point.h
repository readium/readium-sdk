//
//  nav_point.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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

#ifndef __ePub3__nav_point__
#define __ePub3__nav_point__

#include "epub3.h"
#include "nav_element.h"
#include "utfstring.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class NavigationPoint : public NavigationElement
{
public:
                            NavigationPoint()                           = delete;
                            NavigationPoint(xmlNodePtr node); // NB: does NOT cascade to create children
                            NavigationPoint(const std::string& ident, const std::string& label, const std::string& content) : NavigationElement(), _label(label), _content(content) {}
                            NavigationPoint(const NavigationPoint&)     = delete;
                            NavigationPoint(NavigationPoint&& o) : NavigationElement(o), _label(std::move(o._label)), _content(std::move(o._content)) {}
    virtual                 ~NavigationPoint() {}
    
    virtual const string&   Title()                     const   { return _label; }
    virtual void            SetTitle(const string& str)         { _label = str; }
    virtual void            SetTitle(string&& str)              { _label = str; }
    
    const string&           Content()                   const   { return _content; }
    
protected:
    string _label;
    string _content;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_point__) */
