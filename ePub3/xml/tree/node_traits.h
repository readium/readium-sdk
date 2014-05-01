//
//  node_traits.h
//  ePub3
//
//  Created by Jim Dovey on 10/26/2013.
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

#ifndef ePub3_node_traits_h
#define ePub3_node_traits_h

#include <ePub3/xml/base.h>

EPUB3_XML_BEGIN_NAMESPACE

#if EPUB_PLATFORM(WINRT)
enum class NodeType {
	Element						= int(Windows::Data::Xml::Dom::NodeType::ElementNode),
	Attribute					= int(Windows::Data::Xml::Dom::NodeType::AttributeNode),
	Text						= int(Windows::Data::Xml::Dom::NodeType::TextNode),
	CDATASection				= int(Windows::Data::Xml::Dom::NodeType::TextNode)+1,
	EntityReference				= int(Windows::Data::Xml::Dom::NodeType::EntityReferenceNode),
	Entity						= int(Windows::Data::Xml::Dom::NodeType::EntityNode),
	ProcessingInstruction		= int(Windows::Data::Xml::Dom::NodeType::ProcessingInstructionNode),
	Comment						= int(Windows::Data::Xml::Dom::NodeType::CommentNode),
	Document					= int(Windows::Data::Xml::Dom::NodeType::DocumentNode),
	DocumentType				= int(Windows::Data::Xml::Dom::NodeType::DocumentTypeNode),
	DocumentFragment			= int(Windows::Data::Xml::Dom::NodeType::DocumentFragmentNode),
	Notation					= int(Windows::Data::Xml::Dom::NodeType::NotationNode),
	HTMLDocument				= int(Windows::Data::Xml::Dom::NodeType::TextNode)+2,
	DTD							= int(Windows::Data::Xml::Dom::NodeType::TextNode)+3,
	ElementDeclaration			= int(Windows::Data::Xml::Dom::NodeType::TextNode)+4,
	AttributeDeclaration		= int(Windows::Data::Xml::Dom::NodeType::TextNode)+5,
	EntityDeclaration			= int(Windows::Data::Xml::Dom::NodeType::TextNode)+6,
	NamespaceDeclaration		= int(Windows::Data::Xml::Dom::NodeType::TextNode)+7,
	XIncludeStart				= int(Windows::Data::Xml::Dom::NodeType::TextNode)+8,
	XIncludeEnd					= int(Windows::Data::Xml::Dom::NodeType::TextNode)+9,
};
#else
/**
 @ingroup tree
 */
enum class NodeType : uint8_t {
    Element                         = ::XML_ELEMENT_NODE,
    Attribute                       = ::XML_ATTRIBUTE_NODE,
    Text                            = ::XML_TEXT_NODE,
    CDATASection                    = ::XML_CDATA_SECTION_NODE,
    EntityReference                 = ::XML_ENTITY_REF_NODE,
    Entity                          = ::XML_ENTITY_NODE,
    ProcessingInstruction           = ::XML_PI_NODE,
    Comment                         = ::XML_COMMENT_NODE,
    Document                        = ::XML_DOCUMENT_NODE,
    DocumentType                    = ::XML_DOCUMENT_TYPE_NODE,
    DocumentFragment                = ::XML_DOCUMENT_FRAG_NODE,
    Notation                        = ::XML_NOTATION_NODE,
    HTMLDocument                    = ::XML_HTML_DOCUMENT_NODE,
    DTD                             = ::XML_DTD_NODE,
    ElementDeclaration              = ::XML_ELEMENT_DECL,
    AttributeDeclaration            = ::XML_ATTRIBUTE_DECL,
    EntityDeclaration               = ::XML_ENTITY_DECL,
    NamespaceDeclaration            = ::XML_NAMESPACE_DECL,
    XIncludeStart                   = ::XML_XINCLUDE_START,
    XIncludeEnd                     = ::XML_XINCLUDE_END,
#ifdef LIBXML_DOCB_ENABLED
    DocbookSGMLDocument             = ::XML_DOCB_DOCUMENT_NODE,
#endif
};
#endif

EPUB3_XML_END_NAMESPACE

#endif
