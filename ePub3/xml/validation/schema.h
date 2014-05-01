//
//  schema.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-19.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
