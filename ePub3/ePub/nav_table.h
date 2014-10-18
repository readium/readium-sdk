//
//  nav_table.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
private:
                            NavigationTable()                               _DELETED_;
                            NavigationTable(const NavigationTable&)         _DELETED_;

public:
    EPUB3_EXPORT            NavigationTable(shared_ptr<Package>& owner, const string& sourceHref);   // requires a HTML <nav> node
                            NavigationTable(NavigationTable&& o) : NavigationElement(std::move(o)), OwnedBy(std::move(o)), _type(std::move(o._type)), _title(std::move(o._title)), _sourceHref(std::move(o._sourceHref)) {}
    
        
    virtual                 ~NavigationTable() {}
    
    EPUB3_EXPORT
	bool                    ParseXML(shared_ptr<xml::Node> node);

	EPUB3_EXPORT
	bool					ParseNCXNavMap(shared_ptr<xml::Node> node, const string& title);
	EPUB3_EXPORT
	bool					ParseNCXPageList(shared_ptr<xml::Node> node);
	EPUB3_EXPORT
	bool					ParseNCXNavList(shared_ptr<xml::Node> node);
    
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
    
	shared_ptr<NavigationElement>   BuildNavigationPoint(shared_ptr<xml::Node> liNode);
	shared_ptr<NavigationElement>	BuildNCXNavigationPoint(shared_ptr<xml::Node> node);

	void                    LoadChildElements(shared_ptr<NavigationElement> pElement, shared_ptr<xml::Node> pXmlNode);
	void					LoadChildNavPoint(shared_ptr<NavigationElement> pElement, shared_ptr<xml::Node> navPoint);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_table__) */
