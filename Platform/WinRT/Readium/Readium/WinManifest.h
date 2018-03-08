//
//  WinManifest.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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

#ifndef __Readium_Manifest_h__
#define __Readium_Manifest_h__

#include "Readium.h"
#include "PropertyHolderImpl.h"
#include <ePub3/manifest.h>

BEGIN_READIUM_API

ref class Container;
ref class Package;
ref class SpineItem;
ref class CFI;
ref class EncryptionInfo;
interface class IClosableStream;

using ::Platform::String;

public ref class ItemProperties sealed
{
	_DECLARE_BRIDGE_API_(::ePub3::ItemProperties, ItemProperties^);

internal:
	ItemProperties(::ePub3::ItemProperties& native) : _native(native) {
		_native.SetBridge(this);
	}

public:
	ItemProperties(ItemProperties^ o) : _native(o->NativeObject) {
		_native.SetBridge(this);
	}
	virtual ~ItemProperties() {}

	static property UINT None				{ UINT get() { return ::ePub3::ItemProperties::None; } }
	static property UINT CoverImage			{ UINT get() { return ::ePub3::ItemProperties::CoverImage; } }
	static property UINT ContainsMathML		{ UINT get() { return ::ePub3::ItemProperties::ContainsMathML; } }
	static property UINT Navigation			{ UINT get() { return ::ePub3::ItemProperties::Navigation; } }
	static property UINT HasRemoteResources	{ UINT get() { return ::ePub3::ItemProperties::HasRemoteResources; } }
	static property UINT HasScriptedContent	{ UINT get() { return ::ePub3::ItemProperties::HasScriptedContent; } }
	static property UINT ContainsSVG		{ UINT get() { return ::ePub3::ItemProperties::ContainsSVG; } }
	static property UINT ContainsSwitch		{ UINT get() { return ::ePub3::ItemProperties::ContainsSwitch; } }
	static property UINT AllPropertiesMask	{ UINT get() { return ::ePub3::ItemProperties::AllPropertiesMask; } }

	[::Windows::Foundation::Metadata::DefaultOverloadAttribute]
	bool HasProperty(UINT aProperty) { return _native.HasProperty(aProperty); }
	bool HasProperty(ItemProperties^ properties) { return _native.HasProperty(properties->NativeObject); }

	property UINT Value { UINT get() { return (::ePub3::ItemProperties::value_type)_native; } }
	virtual String^ ToString() override { return StringFromNative(_native.str()); }

};

public ref class ManifestItem sealed : public IPropertyHolder
{
	_DECLARE_BRIDGE_API_(::ePub3::ManifestItemPtr, ManifestItem^);

internal:
	ManifestItem(::ePub3::ManifestItemPtr native);

public:
	virtual ~ManifestItem() {}

	property Package^ ParentPackage { Package^ get(); }

	property String^ AbsolutePath { String^ get(); }

	property String^ Identifier { String^ get(); }
	property String^ Href { String^ get(); }
	property String^ MediaType { String^ get(); }

	property String^ MediaOverlayID { String^ get(); }
	property ManifestItem^ MediaOverlay { ManifestItem^ get(); }

	property String^ FallbackID { String^ get(); }
	property ManifestItem^ Fallback { ManifestItem^ get(); }

	property String^ BaseHref { String^ get(); }

	bool HasProperty(ItemProperties^ prop);
	[::Windows::Foundation::Metadata::DefaultOverloadAttribute]
	bool HasProperty(UINT prop);
	bool HasProperty(IVectorView<Uri^>^ prop);

	property EncryptionInfo^ Encryption { EncryptionInfo^ get(); }

	property bool CanLoadDocument { bool get(); }
	
	::Windows::Data::Xml::Dom::XmlDocument^ LoadDocument();

	IClosableStream^ ReadStream();

#include "PropertyHolderSubclassDecl.h"

};

END_READIUM_API

#endif	/* __Readium_Manifest_h__ */
