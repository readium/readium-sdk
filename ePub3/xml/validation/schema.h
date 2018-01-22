//
//  schema.h
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

#ifndef __ePub3_xml_schema__
#define __ePub3_xml_schema__

#include <ePub3/xml/base.h>

#if EPUB_USE(LIBXML2)
#include <ePub3/xml/xmlstring.h>
#include <string>
#include <libxml/xmlschemastypes.h>
#include <libxml/schemasInternals.h>

EPUB3_XML_BEGIN_NAMESPACE

class Document;

/**
 @ingroup validation
 */
class Schema : public WrapperBase<Schema>
{
public:
    explicit Schema(_xmlSchema* schema);
    explicit Schema(std::shared_ptr<Document> document = nullptr, bool assume_ownership = false);
    virtual ~Schema();
    
    virtual void SetDocument(std::shared_ptr<Document> doc = nullptr, bool assume_ownership = false);
    
    string Name() const;
    string TargetNamespace() const;
    string Version() const;
    
    std::shared_ptr<class Document> Document();
    std::shared_ptr<const class Document> Document() const;
    
    _xmlSchema * xmlSchema();
    const _xmlSchema * xmlSchema() const;
    
protected:
    virtual void releaseDocument();
    
private:
    _xmlSchema *    _xml;
    bool            _owns_document;
    
};

EPUB3_XML_END_NAMESPACE
#endif	// EPUB_USE(LIBXML2)

#endif /* defined(__ePub3_xml_schema__) */
