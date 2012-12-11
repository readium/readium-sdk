//
//  schema.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3_xml_schema__
#define __ePub3_xml_schema__

#include "base.h"
#include "xmlstring.h"
#include <string>
#include <libxml/xmlschemastypes.h>

EPUB3_XML_BEGIN_NAMESPACE

class Document;

class Schema : public WrapperBase
{
public:
    explicit Schema(_xmlSchema* schema);
    explicit Schema(Document * document = NULL, bool assume_ownership = false);
    virtual ~Schema();
    
    virtual void SetDocument(Document * doc = NULL, bool assume_ownership = false);
    
    string Name() const;
    string TargetNamespace() const;
    string Version() const;
    
    Document * Document();
    const class Document * Document() const;
    
    _xmlSchema * xmlSchema();
    const _xmlSchema * xmlSchema() const;
    
protected:
    virtual void releaseDocument();
    
private:
    _xmlSchema *    _xml;
    bool            _owns_document;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_schema__) */
