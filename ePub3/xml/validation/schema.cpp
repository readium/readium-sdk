//
//  schema.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

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
