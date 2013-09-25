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

#include <ePub3/epub3.h>
#include <ePub3/nav_point.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/xml/node.h>

EPUB3_BEGIN_NAMESPACE

class Package;
class NavigationTable;

typedef shared_ptr<NavigationTable> NavigationTablePtr;

/**
 @ingroup navigation
 */
class NavigationTable : public NavigationElement, public PointerType<NavigationTable>, public OwnedBy<Package>
{
private:
                            NavigationTable()                               _DELETED_;
                            NavigationTable(const NavigationTable&)         _DELETED_;

public:
    EPUB3_EXPORT            NavigationTable(shared_ptr<Package>& owner, const string& sourceHref);   // requires a HTML <nav> node
                            NavigationTable(NavigationTable&& o) : NavigationElement(std::move(o)), OwnedBy(std::move(o)), _type(std::move(o._type)), _title(std::move(o._title)), _sourceHref(std::move(o._sourceHref)) {}
    
        
    virtual                 ~NavigationTable() {}
    
    EPUB3_EXPORT
    bool                    ParseXML(xml::Node* node);
    
    const string&           Type()                      const   { return _type; }
    void                    SetType(const string& str)          { _type = str; }
    void                    SetType(string&& str)               { _type = str; }
    
    virtual const string&   Title()                     const   { return _title; }
    virtual void            SetTitle(const string& str)         { _title = str; }
    virtual void            SetTitle(string&& str)              { _title = str; }

    const string&           SourceHref()                      const   { return _sourceHref; }
    void                    SetSourceHref(const string& str)    { _sourceHref = str; }
    void                    SetSourceHref(string&& str)         { _sourceHref = str; }
    
protected:
    string      _type;          ///< The type qualifier (toc, lot, loi, ...) for this table.
    string      _title;         ///< The table's title. Optional.
    string      _sourceHref;    ///< Href to the nav item representing the table in the package.
    
    shared_ptr<NavigationElement>   BuildNavigationPoint(xml::Node* liNode);

    void                    LoadChildElements(shared_ptr<NavigationElement> pElement, xml::Node* pXmlNode);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_table__) */
