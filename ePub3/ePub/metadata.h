//
//  metadata.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-04.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
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
