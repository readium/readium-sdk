//
//  collection.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/28/2013.
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
