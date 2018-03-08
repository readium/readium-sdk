//
//  collection.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/28/2013.
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

#include "epub_collection.h"
#include "package.h"
#include <ePub3/epub3.h>
#include <ePub3/utilities/error_handler.h>

EPUB3_BEGIN_NAMESPACE

const string_view Collection::IndexRole("index", 5);
const string_view Collection::ManifestRole("manifest", 8);
const string_view Collection::PreviewRole("preview", 7);

Collection::Collection(const PackagePtr& owner, const CollectionPtr& parent)
    : PointerType(), OwnedBy(owner), XMLIdentifiable(), PropertyHolder(),
      _parentCollection(parent), _role(), _childCollections(), _links()
#if EPUB_PLATFORM(WINRT)
    , NativeBridge()
#endif
{
}
bool Collection::ParseXML(shared_ptr<xml::Node> node)
{
    try
    {
        SetXMLIdentifier(_getProp(node, "id"));
        
        _role = _getProp(node, "role", ePub3NamespaceURI);
        if (_role.empty())
            HandleError(EPUBError::OPFCollectionMissingRole);
        
        size_t nodeIdx = 0;
        for (auto child = node->FirstElementChild(); bool(child); child = child->NextElementSibling(), ++nodeIdx)
        {
            string name(child->Name());
            
            if (name == "metadata")
            {
                if (nodeIdx != 0)
                    HandleError(EPUBError::OPFCollectionMetadataOutOfOrder);
                
                // parse the metadata elements
                ParseMetadata(child);
            }
            else if (name == "collection")
            {
                if (_links.size() != 0 )
                    HandleError(EPUBError::OPFCollectionSubcollectionOutOfOrder);
                
//                CollectionPtr sub = New(Owner(), shared_from_this());
                CollectionPtr sub = std::make_shared<Collection>(Owner(), shared_from_this());
                if (sub->ParseXML(child))
                {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
                    _childCollections.emplace(sub->Role(), sub);
#else
                    _childCollections[sub->Role()] == sub;
#endif
                }
            }
            else if (name == "link")
            {
                LinkPtr link = std::make_shared<Link>(shared_from_this()); //Link::New(shared_from_this());
                if (link->ParseXML(child))
                    _links.push_back(link);
            }
        }
        
        if (_links.empty())
            HandleError(EPUBError::OPFCollectionNoLinks);
    }
    catch (const epub_spec_error&)
    {
        return false;
    }
    
    return true;
}
void Collection::ParseMetadata(shared_ptr<xml::Node> node)
{
    PropertyHolderPtr holderPtr = CastPtr<PropertyHolder>();
    for (auto metaNode = node->FirstElementChild(); bool(metaNode); metaNode = metaNode->NextElementSibling())
    {
        if (!_getProp(metaNode, "refines").empty())
        {
            HandleError(EPUBError::OPFCollectionLinkIncludesRefinement);
            continue;
        }
        
        if (metaNode->Name() == "meta")
        {
            HandleError(EPUBError::OPFCollectionIllegalMetaElement);
            continue;
        }
        
        PropertyPtr p = std::make_shared<Property>(holderPtr); //Property::New(holderPtr);
        if (p->ParseMetaElement(metaNode))
            AddProperty(p);
    }
}

EPUB3_END_NAMESPACE
