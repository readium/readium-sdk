//
//  glossary.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
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

#ifndef __ePub3__glossary__
#define __ePub3__glossary__

#include <ePub3/nav_element.h>
#include <map>

EPUB3_BEGIN_NAMESPACE

/**
 @ingroup navigation
 */
class Glossary : public NavigationElement, public PointerType<Glossary>, public OwnedBy<Package>
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    typedef string                      Term;
    typedef string                      Definition;     // this will change to 'attributed string' or similar
    typedef std::pair<Term, Definition> Entry;          // lookup table uses lowercased terms as keys; value contains case-correct version
    
protected:
    // This should be a Trie-- and will be, when I write one.
    // When that happens, the following public API will be added to the Glossary class:
    //   const std::vector<const Term&> CompletionsForString(const std::string& str) const;
    //   const Term FirstCompletionForString(const std::string& str) const;
    typedef std::map<Term, Entry>       LookupTable;
    
public:
	EPUB3_EXPORT    Glossary(shared_ptr<xml::Node> node, PackagePtr pkg);  // must be a <dl> node with epub:type="glossary"
					Glossary(const string& identifier, PackagePtr pkg) : _ident(identifier), OwnedBy(pkg) {}
					Glossary(string&& identifier, PackagePtr pkg) : _ident(identifier), OwnedBy(pkg) {}
                    Glossary(Glossary&& o) : OwnedBy(std::move(o)), _ident(std::move(o._ident)), _lookup(std::move(o._lookup)) {}
    virtual         ~Glossary() {}

private:
                    Glossary() _DELETED_;
                    Glossary(const Glossary&) _DELETED_;

public:
    virtual const string&   Title()                     const   { return _ident; }
    virtual void            SetTitle(const string& str)         { _ident = str; }
    virtual void            SetTitle(string&& str)              { _ident = str; }
    
    virtual const Entry     Lookup(const Term& term)    const;
    
    // Q: *can* these fail?
    virtual bool            AddDefinition(const Term& term, const Definition& definition);
    virtual bool            AddDefinition(const Term& term, Definition&& definition);
    
protected:
    string _ident;
    LookupTable _lookup;
    
	bool                    Parse(shared_ptr<xml::Node> node);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__glossary__) */
