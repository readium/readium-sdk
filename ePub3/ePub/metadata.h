//
//  metadata.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-04.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#ifndef __ePub3__metadata__
#define __ePub3__metadata__

#include "epub3.h"
#include "../utilities/iri.h"
#include "../utilities/utfstring.h"
#include <vector>
#include <map>
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

class Package;

enum class Direction
{
    Natural,
    LeftToRight,
    RightToLeft,
    VerticalLeftToRight,
    VerticalRightToLeft
};

class Metadata
{
public:
    enum class DCType : uint32_t
    {
        Invalid,
        
        // Required DCMES elements
        Identifier,
        Title,
        Language,
        
        // Optional DCMES elements
        Contributor,
        Coverage,
        Creator,
        Date,
        Description,
        Format,
        Publisher,
        Relation,
        Rights,
        Source,
        Subject,
        Type,
        
        Custom          = UCHAR_MAX     // non-DCMES metadata value
    };
    
    class Extension
    {
    public:
                    Extension()                         = delete;
                    Extension(xmlNodePtr node, const Package* owner);
                    Extension(const Extension&)         = delete;
                    Extension(Extension&&);
        virtual     ~Extension();
        
        const IRI&  Property()          const       { return _property; }
        string      Scheme()            const;
        string      Value()             const;
        string      Identifier()        const;
        string      Language()          const;
        
    protected:
        xmlNodePtr  _node;
        IRI         _property;
    };
    
    typedef std::vector<Extension*>  ExtensionList;
    
public:
                    Metadata()                          = delete;
                    Metadata(xmlNodePtr node, const Package* owner);
                    Metadata(const Metadata&)           = delete;
                    Metadata(Metadata&&);
    virtual         ~Metadata();
    
    DCType                  Type()          const           { return _type; }
    const IRI&              Property()      const           { return _property; }
    string                  Identifier()    const;
    string                  Value()         const;
    string                  Language()      const;
    
    const ExtensionList&    Extensions()    const           { return _extensions; }
    const Extension*        ExtensionWithProperty(const IRI& property) const;
    
    void                    AddExtension(xmlNodePtr node, const Package* owner);
    
    static const IRI        IRIForDCType(DCType type);
    
public:
    // Some things to help with debugging
    typedef std::vector<std::pair<string, string>>   ValueMap;
    
    const ValueMap          DebugValues()   const;
    
protected:
    DCType          _type;
    xmlNodePtr      _node;
    ExtensionList   _extensions;
    IRI             _property;
    
    bool            Decode(const Package* owner);
    
    static std::map<string, DCType> NameToIDMap;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__metadata__) */
