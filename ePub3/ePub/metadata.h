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
#include <vector>
#include <string>
#include <libxml/tree.h>

EPUB3_BEGIN_NAMESPACE

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
    enum class DCType : uint8_t
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
        Type
    };
    
    class Extension
    {
    public:
        Extension() = delete;
        Extension(xmlNodePtr node);
        Extension(const Extension&) = delete;
        Extension(Extension&&);
        virtual ~Extension();
        
        std::string Property() const;
        std::string Scheme() const;
        std::string Value() const;
        std::string Identifier() const;
        std::string Language() const;
        
    protected:
        xmlNodePtr  _node;
    };
    
    typedef std::vector<Extension>  ExtensionList;
    
public:
    Metadata() = delete;
    Metadata(xmlNodePtr node);
    Metadata(const Metadata&) = delete;
    Metadata(Metadata&&);
    virtual ~Metadata();
    
    DCType Type() const { return _type; }
    std::string Name() const;
    std::string Identifier() const;
    std::string Value() const;
    std::string Language() const;
    
    const ExtensionList& Extensions() const { return _ext; }
    
    void AddExtension(xmlNodePtr node);
    
protected:
    xmlNodePtr      _node;
    DCType          _type;
    ExtensionList   _ext;
    
    bool Decode();
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__metadata__) */
