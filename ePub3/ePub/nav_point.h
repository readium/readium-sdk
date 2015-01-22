//
//  nav_point.h
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

#ifndef __ePub3__nav_point__
#define __ePub3__nav_point__

#include <ePub3/epub3.h>
#include <ePub3/nav_element.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/owned_by.h>
#include <vector>
#include <memory>

EPUB3_BEGIN_NAMESPACE

class NavigationPoint;

typedef shared_ptr<NavigationPoint> NavigationPointPtr;

/**
 @ingroup navigation
 */
class NavigationPoint : public NavigationElement, public PointerType<NavigationPoint>, public OwnedBy<NavigationElement>
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
private:
                            NavigationPoint(const NavigationPoint&)     _DELETED_;
public:
                            NavigationPoint(shared_ptr<NavigationElement>& owner) : OwnedBy(owner) {};
                            NavigationPoint(shared_ptr<NavigationElement>& owner, const std::string& ident, const std::string& label, const std::string& content) : NavigationElement(), OwnedBy(owner), _label(label), _content(content) {}
                            NavigationPoint(NavigationPoint&& o) : NavigationElement(std::move(o)), OwnedBy(std::move(o)), _label(std::move(o._label)), _content(std::move(o._content)) {}
    virtual                 ~NavigationPoint() {}
    
    virtual const string&   Title()                     const   { return _label; }
    virtual void            SetTitle(const string& str)         { _label = str; }
    virtual void            SetTitle(string&& str)              { _label = str; }
    
    const string&           Content()                   const   { return _content; }
    void                    SetContent(const string& str)       { _content = str; }
    void                    SetContent(string&& str)            { _content = str; }

	///
	/// Turns the Content() href into an absolute value, similar to ManifestItem::AbsolutePath().
	EPUB3_EXPORT
	string					AbsolutePath(ConstPackagePtr pkg)				const;
    
protected:
    string _label;
    string _content;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_point__) */
