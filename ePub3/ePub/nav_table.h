//
//  nav_table.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

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
