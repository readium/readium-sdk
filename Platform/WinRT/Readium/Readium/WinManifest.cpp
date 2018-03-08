//
//  WinManifest.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-10.
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

#include "WinManifest.h"
#include "WinPackage.h"
#include "WinEncryptionInfo.h"
#include "Streams.h"
#include "CollectionBridges.h"
#include <ePub3/xml/document.h>

using namespace ::Platform;
using namespace ::Windows::Data::Xml::Dom;
using namespace ::Windows::Foundation;
using namespace ::Windows::Foundation::Collections;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::ManifestItemPtr, ManifestItem)

ManifestItem::ManifestItem(::ePub3::ManifestItemPtr native) : _native(native)
{
	_native->SetBridge(this);
}

Package^ ManifestItem::ParentPackage::get()
{
	return Package::Wrapper(_native->GetPackage());
}

String^ ManifestItem::AbsolutePath::get()
{
	return StringFromNative(_native->AbsolutePath());
}

String^ ManifestItem::Identifier::get()
{
	return StringFromNative(_native->Identifier());
}
String^ ManifestItem::Href::get()
{
	return StringFromNative(_native->Href());
}
String^ ManifestItem::MediaType::get()
{
	return StringFromNative(_native->MediaType());
}

String^ ManifestItem::MediaOverlayID::get()
{
	return StringFromNative(_native->MediaOverlayID());
}
ManifestItem^ ManifestItem::MediaOverlay::get()
{
	return Wrapper(_native->MediaOverlay());
}

String^ ManifestItem::FallbackID::get()
{
	return StringFromNative(_native->FallbackID());
}
ManifestItem^ ManifestItem::Fallback::get()
{
	return Wrapper(_native->Fallback());
}

String^ ManifestItem::BaseHref::get()
{
	return StringFromNative(_native->BaseHref());
}

bool ManifestItem::HasProperty(ItemProperties^ prop)
{
	return _native->HasProperty(prop->NativeObject);
}
bool ManifestItem::HasProperty(UINT prop)
{
	return _native->HasProperty(prop);
}
bool ManifestItem::HasProperty(IVectorView<Uri^>^ prop)
{
	std::vector<::ePub3::IRI> propertyIRIs;
	propertyIRIs.reserve(prop->Size);
	for (auto uri : prop)
	{
		propertyIRIs.emplace_back(URIToIRI(uri));
	}
	return _native->HasProperty(propertyIRIs);
}

EncryptionInfo^ ManifestItem::Encryption::get()
{
	return EncryptionInfo::Wrapper(_native->GetEncryptionInfo());
}

bool ManifestItem::CanLoadDocument::get()
{
	return _native->CanLoadDocument();
}

::Windows::Data::Xml::Dom::XmlDocument^ ManifestItem::LoadDocument()
{
	return _native->ReferencedDocument()->xml();
}

IClosableStream^ ManifestItem::ReadStream()
{
	return ref new Stream(_native->Reader());
}

#define PropertyHolder ManifestItem
#include "PropertyHolderSubclassImpl.h"
#undef PropertyHolder

END_READIUM_API
