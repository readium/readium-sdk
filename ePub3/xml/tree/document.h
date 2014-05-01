//
//  document.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
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

#ifndef __ePub3_xml_document__
#define __ePub3_xml_document__

#include <ePub3/xml/node.h>
#include <ePub3/xml/document.h>

#if EPUB_USE(LIBXML2)
#include <ePub3/xml/io.h>
#include <ePub3/xml/c14n.h>
#endif

EPUB3_XML_BEGIN_NAMESPACE

class DTD;
class Element;
#if EPUB_USE(LIBXML2)
class C18N;
#endif

/**
 @ingroup tree
 */
enum class EntityType {
    InternalGeneral,
    ExternalGeneralParsed,
    ExternalGeneralUnparsed,
    InternalParameterEntity,
    ExternalParameterEntity,
    InternalPredefinedEntity,
};

/**
 @ingroup tree
 */
class Document : public Node
{
public:
#if EPUB_USE(LIBXML2)
	typedef _xmlDoc*								NativeDocPtr;
	typedef xmlEntityPtr							NativeEntityPtr;
#elif EPUB_USE(WIN_XML)
	typedef Windows::Data::Xml::Dom::XmlDocument^	NativeDocPtr;
	typedef Windows::Data::Xml::Dom::DtdEntity^		NativeEntityPtr;
#endif

public:
	explicit Document(NativeDocPtr xml);
#if EPUB_ENABLE(XML_BUILDER)
    explicit Document(const string & version = "1.0");
    explicit Document(std::shared_ptr<Element> rootElement);
#endif
    virtual ~Document();
    
    string Encoding() const;
    std::shared_ptr<DTD> InternalSubset() const;
#if EPUB_ENABLE(XML_BUILDER)
    void SetInternalSubset(const string & name, const string & externalID,
                           const string & systemID);
    
#endif

	std::shared_ptr<Element> Root();
	std::shared_ptr<const Element> Root() const;
#if EPUB_ENABLE(XML_BUILDER)
	std::shared_ptr<Element> SetRoot(const string & name, const string & nsUri = string(),
                      const string & nsPrefix = string());
	std::shared_ptr<Element> SetRoot(std::shared_ptr<const Node> nodeToCopy, bool recursive = true);
	std::shared_ptr<Element> SetRoot(std::shared_ptr<Element> root);
#endif

#if EPUB_ENABLE(XML_BUILDER)
    // only comments and processing instructions can be outside the root node;
    // this will throw if another type of node is passed in-- it checks the node's Type()
    std::shared_ptr<Node> AddNode(std::shared_ptr<Node> commentOrPINode, bool beforeRoot = true);
    
    std::shared_ptr<Node> AddComment(const string & comment, bool beforeRoot = true);
    std::shared_ptr<Node> AddProcessingInstruction(const string & name, const string & content, bool beforeRoot = true);
    
    // add an entity declaration to a given subset
    void DeclareEntity(const string & name, EntityType type, const string & publicID, const string & systemID, const string & value);
#endif

#if EPUB_USE(LIBXML2)
    int ProcessXInclude(bool generateXIncludeNodes = true);
#endif

    NativeDocPtr xml() { return xml_native_cast<NativeDocPtr>(Node::xml()); }
    const NativeDocPtr xml() const { return xml_native_cast<const NativeDocPtr>(Node::xml()); }
    
    NativeEntityPtr NamedEntity(const string & name) const;
    string ContentOfNamedEntity(const string & name) const;
    
    //////////////////////////////////////////////////////////////////////////////
    // Output
#if EPUB_USE(LIBXML2)
#if EPUB_ENABLE(XML_C14N)
    template <C14NVersion _Version, bool _WithComments>
    string Canonicalize(const C14NParams<_Version, _WithComments> & params) const {
        return "";
    }
#endif

    void WriteXML(OutputBuffer & outbuf) const;
#endif
    void WriteXML(string & string) const;
    
    string XMLString() const { string __s; WriteXML(__s); return __s; }
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_document__) */
