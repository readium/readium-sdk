//
//  schema.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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
