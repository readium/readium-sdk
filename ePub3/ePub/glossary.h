//
//  glossary.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
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
