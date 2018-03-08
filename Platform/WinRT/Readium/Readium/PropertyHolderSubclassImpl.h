//
//  PropertyHolderSubclassDecl.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-10-04.
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

// Include this inside the definition file of a class which should implement IPropertyHolder
// I'm using this as a cut-and-paste method of writing a crapload of code

#ifndef PropertyHolder
# error "Please define 'PropertyHolder' to be the correct class name"
#endif

unsigned int PropertyHolder::Count::get()
{
	return static_cast<unsigned int>(Native->NumberOfProperties());
}

void PropertyHolder::Append(Property^ prop)
{
	Native->AddProperty(prop->NativeObject);
}
void PropertyHolder::Append(IPropertyHolder^ allProps)
{
	auto input = dynamic_cast<PropertyHolder^>(allProps)->Native;
	Native->AppendProperties(*input.get(), Native);
}

void PropertyHolder::Remove(Uri^ propertyIRI)
{
	Native->RemoveProperty(URIToIRI(propertyIRI));
}
void PropertyHolder::Remove(String^ reference)
{
	Native->RemoveProperty(StringToNative(reference));
}
void PropertyHolder::Remove(String^ prefix, String^ reference)
{
	// yes, the parameters are the other way around on the native C++ object
	Native->RemoveProperty(StringToNative(reference), StringToNative(prefix));
}

Property^ PropertyHolder::At(unsigned int idx)
{
	auto prop = Native->PropertyAt(idx);
	return Property::Wrapper(prop);
}
void PropertyHolder::EraseAt(unsigned int idx)
{
	Native->ErasePropertyAt(idx);
}

bool PropertyHolder::Contains(DCType type)
{
	return Native->ContainsProperty(::ePub3::DCType(type));
}
bool PropertyHolder::Contains(Uri^ propertyIRI)
{
	return Native->ContainsProperty(URIToIRI(propertyIRI));
}
bool PropertyHolder::Contains(String^ reference)
{
	return Native->ContainsProperty(StringToNative(reference));
}
bool PropertyHolder::Contains(String^ prefix, String^ reference)
{
	return Native->ContainsProperty(StringToNative(reference), StringToNative(prefix));
}

Property^ PropertyHolder::PropertyMatching(DCType type)
{
	auto prop = Native->PropertyMatching(::ePub3::DCType(type));
	return Property::Wrapper(prop);
}
Property^ PropertyHolder::PropertyMatching(Uri^ propertyIRI)
{
	auto prop = Native->PropertyMatching(URIToIRI(propertyIRI));
	return Property::Wrapper(prop);
}
Property^ PropertyHolder::PropertyMatching(String^ reference)
{
	auto prop = Native->PropertyMatching(StringToNative(reference));
	return Property::Wrapper(prop);
}
Property^ PropertyHolder::PropertyMatching(String^ prefix, String^ reference)
{
	auto prop = Native->PropertyMatching(StringToNative(reference), StringToNative(prefix));
	return Property::Wrapper(prop);
}

IVectorView<Property^>^ PropertyHolder::PropertiesMatching(DCType type)
{
	auto props = Native->PropertiesMatching(::ePub3::DCType(type));
	return ref new PropertyVectorView(props);
}
IVectorView<Property^>^ PropertyHolder::PropertiesMatching(Uri^ propertyIRI)
{
	auto props = Native->PropertiesMatching(URIToIRI(propertyIRI));
	return ref new PropertyVectorView(props);
}
IVectorView<Property^>^ PropertyHolder::PropertiesMatching(String^ reference)
{
	auto props = Native->PropertiesMatching(StringToNative(reference));
	return ref new PropertyVectorView(props);
}
IVectorView<Property^>^ PropertyHolder::PropertiesMatching(String^ prefix, String^ reference)
{
	auto props = Native->PropertiesMatching(StringToNative(reference), StringToNative(prefix));
	return ref new PropertyVectorView(props);
}

void PropertyHolder::RegisterPrefixIRIStem(String^ prefix, String^ iriStem)
{
	Native->RegisterPrefixIRIStem(StringToNative(prefix), StringToNative(iriStem));
}

Uri^ PropertyHolder::MakePropertyIRI(String^ reference)
{
	return IRIToURI(Native->MakePropertyIRI(StringToNative(reference)));
}
Uri^ PropertyHolder::MakePropertyIRI(String^ prefix, String^ reference)
{
	return IRIToURI(Native->MakePropertyIRI(StringToNative(reference), StringToNative(prefix)));
}
Uri^ PropertyHolder::PropertyIRIFromString(String^ str)
{
	return IRIToURI(Native->PropertyIRIFromString(StringToNative(str)));
}

IIterator<Property^>^ PropertyHolder::First()
{
	return ref new PropertyIteratorImpl(this);
}
