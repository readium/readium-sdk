//
//  WinManifest.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-26.
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
#if EPUB_PLATFORM(WIN_PHONE)
	String^ ToString() { return StringFromNative(_native.str()); }
#else
	virtual String^ ToString() override { return StringFromNative(_native.str()); }
#endif

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
	property UINT ResourceSize { UINT get(); }
	
#if EPUB_USE(WIN_XML)
	::Windows::Data::Xml::Dom::IXmlDocument^
#elif EPUB_USE(WIN_PHONE_XML)
	::PhoneSupportInterfaces::IXmlDocument^
#endif
		LoadDocument();

	IClosableStream^ ReadStream();

#include "PropertyHolderSubclassDecl.h"

};

END_READIUM_API

#endif	/* __Readium_Manifest_h__ */