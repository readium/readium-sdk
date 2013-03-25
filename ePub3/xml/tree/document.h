//
//  document.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
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

#ifndef __ePub3_xml_document__
#define __ePub3_xml_document__

#include "node.h"
#include "element.h"
#include "io.h"
#include "c14n.h"

EPUB3_XML_BEGIN_NAMESPACE

class DTD;
class Element;
class C18N;

enum class EntityType : uint8_t {
    InternalGeneral,
    ExternalGeneralParsed,
    ExternalGeneralUnparsed,
    InternalParameterEntity,
    ExternalParameterEntity,
    InternalPredefinedEntity,
};

class Document : public Node
{
public:
    explicit Document(const string & version = "1.0");
    explicit Document(xmlDocPtr xml);
    explicit Document(Element * rootElement);
    virtual ~Document();
    
    string Encoding() const;
    DTD * InternalSubset() const;
    void SetInternalSubset(const string & name, const string & externalID,
                           const string & systemID);
    
    Element * Root();
    const Element * Root() const;
    
    Element * SetRoot(const string & name, const string & nsUri = string(),
                      const string & nsPrefix = string());
    Element * SetRoot(const Node * nodeToCopy, bool recursive = true);
    Element * SetRoot(Element * root);

    // only comments and processing instructions can be outside the root node;
    // this will throw if another type of node is passed in-- it checks the node's Type()
    Node * AddNode(Node * commentOrPINode, bool beforeRoot = true);
    
    Node * AddComment(const string & comment, bool beforeRoot = true);
    Node * AddProcessingInstruction(const string & name, const string & content, bool beforeRoot = true);
    
    // add an entity declaration to a given subset
    void DeclareEntity(const string & name, EntityType type, const string & publicID, const string & systemID, const string & value);
    
    int ProcessXInclude(bool generateXIncludeNodes = true);
    
    _xmlDoc * xml() { return reinterpret_cast<_xmlDoc*>(Node::xml()); }
    const _xmlDoc * xml() const { return reinterpret_cast<const _xmlDoc*>(Node::xml()); }
    
    xmlEntityPtr NamedEntity(const string & name) const;
    string ContentOfNamedEntity(const string & name) const;
    
    //////////////////////////////////////////////////////////////////////////////
    // Output
    
    template <C14NVersion _V, bool _C>
    string Canonicalize(const C14NParams<_V, _C> & params) const;
    
    void WriteXML(OutputBuffer & outbuf) const;
    void WriteXML(string & string) const;
    
    string XMLString() const { string __s; WriteXML(__s); return std::move(__s); }
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_document__) */
