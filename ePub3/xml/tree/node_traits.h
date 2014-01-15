//
//  node_traits.h
//  ePub3
//
//  Created by Jim Dovey on 10/26/2013.
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

#ifndef ePub3_node_traits_h
#define ePub3_node_traits_h

#include <ePub3/xml/base.h>

EPUB3_XML_BEGIN_NAMESPACE

#if EPUB_PLATFORM(WINRT)
#if EPUB_USE(WIN_PHONE_XML)
typedef ::PhoneSupportInterfaces::NodeType	_BaseNodeType;
#else	// EPUB_USE(WIN_XML)
typedef ::Windows::Data::Xml::Dom::NodeType	_BaseNodeType;
#endif
enum class NodeType {
	Element						= int(_BaseNodeType::ElementNode),
	Attribute					= int(_BaseNodeType::AttributeNode),
	Text						= int(_BaseNodeType::TextNode),
	CDATASection				= int(_BaseNodeType::TextNode)+1,
	EntityReference				= int(_BaseNodeType::EntityReferenceNode),
	Entity						= int(_BaseNodeType::EntityNode),
	ProcessingInstruction		= int(_BaseNodeType::ProcessingInstructionNode),
	Comment						= int(_BaseNodeType::CommentNode),
	Document					= int(_BaseNodeType::DocumentNode),
	DocumentType				= int(_BaseNodeType::DocumentTypeNode),
	DocumentFragment			= int(_BaseNodeType::DocumentFragmentNode),
	Notation					= int(_BaseNodeType::NotationNode),
	HTMLDocument				= int(_BaseNodeType::TextNode)+2,
	DTD							= int(_BaseNodeType::TextNode)+3,
	ElementDeclaration			= int(_BaseNodeType::TextNode)+4,
	AttributeDeclaration		= int(_BaseNodeType::TextNode)+5,
	EntityDeclaration			= int(_BaseNodeType::TextNode)+6,
	NamespaceDeclaration		= int(_BaseNodeType::TextNode)+7,
	XIncludeStart				= int(_BaseNodeType::TextNode)+8,
	XIncludeEnd					= int(_BaseNodeType::TextNode)+9,
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
