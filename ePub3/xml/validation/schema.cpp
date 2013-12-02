//
//  schema.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
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

#include <ePub3/xml/schema.h>
#include <ePub3/xml/document.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

EPUB3_XML_BEGIN_NAMESPACE

Schema::Schema(_xmlSchema * xml) : _xml(xml), _owns_document(false)
{
    //_xml->_private = this;
}
Schema::Schema(std::shared_ptr<class Document> document, bool assume_ownership) : _xml(NULL), _owns_document(false)
{
    SetDocument(document, assume_ownership);
}
Schema::~Schema()
{
    releaseDocument();
}
void Schema::SetDocument(std::shared_ptr<class Document> document, bool assume_ownership)
{
    releaseDocument();
    
    bool created = false;
    if ( !bool(document) )
    {
        document = std::make_shared<class Document>();
        created = true;
    }
    
    xmlResetLastError();
    xmlSchemaParserCtxtPtr ctx = NULL;//xmlSchemaNewDocParserCtxt(document->xmlDoc());
    if ( ctx == NULL )
    {
        throw ParserError("Failed to parse schema: ", xmlGetLastError());
    }
    
    if (_xml != nullptr)
    {
        Node::Unwrap((xmlNodePtr)_xml);
        xmlFreeNode((xmlNodePtr)_xml);
        _xml = nullptr;
    }
    
    _xml = xmlSchemaParse(ctx);
    xmlSchemaFreeParserCtxt(ctx);
    
    if ( _xml == nullptr )
    {
        throw ParserError("Schema could not be parsed: ", xmlGetLastError());
    }
    
    Rewrap(_xml, shared_from_this());
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
        // unwrap the associated document
        Node::Unwrap((xmlNodePtr)_xml->doc);
        _owns_document = false;
    }
    
    if ( _xml != NULL )
    {
        xmlSchemaFree(_xml);
        _xml = NULL;
    }
}
std::shared_ptr<Document> Schema::Document()
{
    if ( _xml == NULL || _xml->doc == NULL )
        return NULL;
    
    return Wrapped<class Document, _xmlDoc>(_xml->doc);
}
std::shared_ptr<const class Document> Schema::Document() const
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
