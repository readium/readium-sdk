//
//  WinPropertyExtension.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#include "WinPropertyExtension.h"
#include "WinProperty.h"

using namespace ::Platform;
using namespace ::Windows::Foundation;

BEGIN_READIUM_API

_BRIDGE_API_IMPL_(::ePub3::PropertyExtensionPtr, PropertyExtension)

PropertyExtension::PropertyExtension(::ePub3::PropertyExtensionPtr native) : _native(native)
{
	_native->SetBridge(this);
}
PropertyExtension::PropertyExtension(Property^ owner) : _native(::ePub3::PropertyExtension::New(owner->NativeObject))
{
	_native->SetBridge(this);
}

Uri^ PropertyExtension::PropertyIdentifier::get()
{
	return IRIToURI(_native->PropertyIdentifier());
}
void PropertyExtension::PropertyIdentifier::set(Uri^ value)
{
	_native->SetPropertyIdentifier(URIToIRI(value));
}

String^ PropertyExtension::Scheme::get()
{
	return StringFromNative(_native->Scheme());
}
void PropertyExtension::Scheme::set(String^ value)
{
	_native->SetScheme(std::move(StringToNative(value)));
}

String^ PropertyExtension::Value::get()
{
	return StringFromNative(_native->Value());
}
void PropertyExtension::Value::set(String^ value)
{
	_native->SetValue(std::move(StringToNative(value)));
}

String^ PropertyExtension::Language::get()
{
	return StringFromNative(_native->Language());
}
void PropertyExtension::Language::set(String^ value)
{
	_native->SetLanguage(std::move(StringToNative(value)));
}

END_READIUM_API
