//
//  glossary.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
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

#ifndef __ePub3__glossary__
#define __ePub3__glossary__

#include "nav_element.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

class Glossary : public NavigationElement
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
                Glossary(xmlNodePtr node);  // must be a <dl> node with epub:type="glossary"
                Glossary(const string& identifier) : _ident(identifier) {}
                Glossary(string&& identifier) : _ident(identifier) {}
                Glossary(Glossary&& o) : _ident(std::move(o._ident)), _lookup(std::move(o._lookup)) {}
    virtual     ~Glossary() {}
    
                Glossary() = delete;
                Glossary(const Glossary&) = delete;
    
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
    
    bool                    Parse(xmlNodePtr node);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__glossary__) */
