//
//  nav_point.h
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
	string					AbsolutePath(ConstPackagePtr pkg)				const;
    
protected:
    string _label;
    string _content;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_point__) */
