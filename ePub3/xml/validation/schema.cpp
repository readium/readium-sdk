//
//  schema.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "schema.h"
#include "document.h"
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

EPUB3_XML_BEGIN_NAMESPACE

Schema::Schema(_xmlSchema * xml) : _xml(xml), _owns_document(false)
{
    _xml->_private = this;
}
Schema::Schema(class Document * document, bool assume_ownership) : _xml(NULL), _owns_document(false)
{
    SetDocument(document, assume_ownership);
}
Schema::~Schema()
{
    releaseDocument();
}
void Schema::SetDocument(class Document * document, bool assume_ownership)
{
    releaseDocument();
    
    bool created = false;
    if ( document == NULL )
    {
        //document = new Document();
        created = true;
    }
    
    xmlResetLastError();
    xmlSchemaParserCtxtPtr ctx = NULL;//xmlSchemaNewDocParserCtxt(document->xmlDoc());
    if ( ctx == NULL )
    {
        if ( created )
            delete document;
        throw ParserError("Failed to parse schema: ", xmlGetLastError());
    }
    
    _xml = xmlSchemaParse(ctx);
    xmlSchemaFreeParserCtxt(ctx);
    
    if ( _xml == NULL )
    {
        if ( created )
            delete document;
        throw ParserError("Schema could not be parsed: ", xmlGetLastError());
    }
    
    _xml->_private = this;
    _owns_document = assume_ownership || created;
}
string Schema::Name() const
{
    if ( _xml == NULL )
        return string();
    return _xml->name;
}
string Schema::TargetNamespace() const
{
    if ( _xml == NULL )
        return string();
    return _xml->targetNamespace;
}
string Schema::Version() const
{
    if ( _xml == NULL )
        return string();
    return _xml->version;
}
void Schema::releaseDocument()
{
    if ( _owns_document && _xml != NULL && _xml->doc != NULL && _xml->doc->_private != NULL )
    {
        // delete the document's associated C++ object
        delete reinterpret_cast<class Document *>(_xml->doc->_private);
        _owns_document = false;
    }
    
    if ( _xml != NULL )
    {
        xmlSchemaFree(_xml);
        _xml = NULL;
    }
}
Document * Schema::Document()
{
    if ( _xml == NULL || _xml->doc == NULL )
        return NULL;
    
    return Wrapped<class Document, _xmlDoc>(_xml->doc);
}
const Document * Schema::Document() const
{
    return const_cast<Schema*>(this)->Document();
}
_xmlSchema * Schema::xmlSchema()
{
    return _xml;
}
const _xmlSchema * Schema::xmlSchema() const
{
    return _xml;
}

EPUB3_XML_END_NAMESPACE
