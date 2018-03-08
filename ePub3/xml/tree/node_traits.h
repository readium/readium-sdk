//
//  node_traits.h
//  ePub3
//
//  Created by Jim Dovey on 10/26/2013.
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
