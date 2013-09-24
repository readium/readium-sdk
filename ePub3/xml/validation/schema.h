//
//  schema.h
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
#endif	// EPUB_USE(LIBXML2)

#endif /* defined(__ePub3_xml_schema__) */
