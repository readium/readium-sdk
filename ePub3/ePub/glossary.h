//
//  glossary.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-13.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__glossary__
#define __ePub3__glossary__

#include "nav_element.h"
#include <map>

EPUB3_BEGIN_NAMESPACE

class Glossary : public NavigationElement
{
public:
    typedef std::string                 Term;
    typedef std::string                 Definition;     // this will change to 'attributed string' or similar
    typedef std::pair<Term, Definition> Entry;          // lookup table uses lowercased terms as keys; value contains case-correct version
    
protected:
    // This should be a Trie-- and will be, when I write one.
    // When that happens, the following public API will be added to the Glossary class:
    //   const std::vector<const Term&> CompletionsForString(const std::string& str) const;
    //   const Term FirstCompletionForString(const std::string& str) const;
    typedef std::map<Term, Entry>       LookupTable;
    
public:
    Glossary(xmlNodePtr node);  // must be a <dl> node with epub:type="glossary"
    Glossary(const std::string& identifier) : _ident(identifier) {}
    Glossary(std::string&& identifier) : _ident(identifier) {}
    Glossary(Glossary&& o) : _ident(std::move(o._ident)), _lookup(std::move(o._lookup)) {}
    virtual ~Glossary() {}
    
    Glossary() = delete;
    Glossary(const Glossary&) = delete;
    
    virtual const std::string& Title() const { return _ident; }
    virtual void SetTitle(const std::string& str) { _ident = str; }
    virtual void SetTitle(std::string&& str) { _ident = str; }
    
    virtual const Entry Lookup(const Term& term) const;
    
    // Q: *can* these fail?
    virtual bool AddDefinition(const Term& term, const Definition& definition);
    virtual bool AddDefinition(const Term& term, Definition&& definition);
    
protected:
    std::string _ident;
    LookupTable _lookup;
    
    bool Parse(xmlNodePtr node);
    static Term Lowercase(const Term& term);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__glossary__) */
