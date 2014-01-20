//
//  WinProperty.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#include "WinProperty.h"
#include "WinPropertyExtension.h"
#include "CollectionBridges.h"
#include "IPropertyHolder.h"
#include "PropertyHolderImpl.h"
#include "WinPackage.h"
#include "WinSpine.h"
#include "WinManifest.h"

using namespace ::Platform;
using namespace ::Windows::Foundation;
using namespace ::Windows::Foundation::Collections;

BEGIN_READIUM_API

typedef BRIDGED_OBJECT_VECTOR(PropertyExtension^, ::ePub3::PropertyExtensionPtr) ExtensionList;

static ::ePub3::PropertyHolderPtr GetNative(IPropertyHolder^ holder)
{
	Package^ pkg = dynamic_cast<Package^>(holder);
	if (pkg != nullptr)
		return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(pkg->NativeObject);

	SpineItem^ spine = dynamic_cast<SpineItem^>(holder);
	if (spine != nullptr)
		return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(spine->NativeObject);

	ManifestItem^ manifest = dynamic_cast<ManifestItem^>(holder);
	if (manifest != nullptr)
		return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(manifest->NativeObject);

	return nullptr;
}

_BRIDGE_API_IMPL_(::ePub3::PropertyPtr, Property)

Property::Property(::ePub3::PropertyPtr native) : _native(native)
{
	_native->SetBridge(this);
}

Property::Property(IPropertyHolder^ holder) : _native(ePub3::Property::New(GetNative(holder)))
{
	_native->SetBridge(this);
}
Property::Property(IPropertyHolder^ holder, Uri^ type, String^ value) : _native(ePub3::Property::New(GetNative(holder)))
{
	_native->SetBridge(this);
	_native->SetPropertyIdentifier(URIToIRI(type));
	_native->SetValue(StringToNative(value));
}

Uri^ Property::IRIForDCType(DCType type)
{
	return IRIToURI(::ePub3::IRIForDCType(::ePub3::DCType(type)));
}
DCType Property::DCTypeFromIRI(Uri^ iri)
{
	return DCType(::ePub3::DCTypeFromIRI(URIToIRI(iri)));
}

DCType Property::Type::get()
{
	return DCType(_native->Type());
}
void Property::Type::set(DCType type)
{
	_native->SetDCType(::ePub3::DCType(type));
}

Uri^ Property::PropertyIdentifier::get()
{
	return IRIToURI(_native->PropertyIdentifier());
}
void Property::PropertyIdentifier::set(Uri^ value)
{
	_native->SetPropertyIdentifier(URIToIRI(value));
}

String^ Property::Value::get()
{
	return StringFromNative(_native->Value());
}
void Property::Value::set(String^ value)
{
	_native->SetValue(std::move(StringToNative(value)));
}

String^ Property::Language::get()
{
	return StringFromNative(_native->Language());
}
void Property::Language::set(String^ value)
{
	_native->SetLanguage(std::move(StringToNative(value)));
}

String^ Property::LocalizedValue::get()
{
	return StringFromNative(_native->LocalizedValue());
}

IVectorView<PropertyExtension^>^ Property::Extensions::get()
{
	return ref new ExtensionList(_native->Extensions());
}

PropertyExtension^ Property::ExtensionWithIdentifier(Uri^ identifier)
{
	return PropertyExtension::Wrapper(_native->ExtensionWithIdentifier(URIToIRI(identifier)));
}
IVectorView<PropertyExtension^>^ Property::AllExtensionsWithIdentifier(Uri^ identifier)
{
	return ref new ExtensionList(_native->AllExtensionsWithIdentifier(URIToIRI(identifier)));
}

void Property::AddExtension(PropertyExtension^ ext)
{
	_native->AddExtension(ext->NativeObject);
}
bool Property::HasExtensionWithIdentifier(Uri^ identifier)
{
	return _native->HasExtensionWithIdentifier(URIToIRI(identifier));
}

END_READIUM_API
