//
//  WinProperty.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

using ExtensionList = BridgedObjectVectorView<PropertyExtension^, ::ePub3::PropertyExtensionPtr>;

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
