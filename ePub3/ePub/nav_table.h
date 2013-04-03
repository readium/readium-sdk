//
//  nav_table.h
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

#ifndef __ePub3__nav_table__
#define __ePub3__nav_table__

#include "epub3.h"
#include "nav_point.h"
#include <libxml/xpath.h> // for xmlNodeSetPtr

EPUB3_BEGIN_NAMESPACE

class NavigationTable : public NavigationElement
{
public:
                            NavigationTable()                               = delete;
                            NavigationTable(xmlNodePtr node);   // requires a HTML <nav> node
                            NavigationTable(const string& type) : NavigationElement(), _type(type), _title() {}
                            NavigationTable(std::string&& type) : NavigationElement(), _type(type), _title() {}
                            NavigationTable(const NavigationTable&)         = delete;
                            NavigationTable(NavigationTable&& o) : NavigationElement(o), _type(std::move(o._type)), _title(std::move(o._title)) {}
    virtual                 ~NavigationTable() {}
    
    const string&           Type()                      const   { return _type; }
    void                    SetType(const string& str)          { _type = str; }
    void                    SetType(string&& str)               { _type = str; }
    
    virtual const string&   Title()                     const   { return _title; }
    virtual void            SetTitle(const string& str)         { _title = str; }
    virtual void            SetTitle(string&& str)              { _title = str; }
    
protected:
    string      _type;
    string      _title;     // optional
    
    bool                    Parse(xmlNodePtr node);
    NavigationElement*      BuildNavigationPoint(xmlNodePtr liNode);

    void                    LoadChildElements(NavigationElement *pElement, xmlNodePtr pXmlNode);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_table__) */
