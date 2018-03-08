//
//  nav_element.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-12.
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
