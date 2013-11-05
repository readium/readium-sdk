//
//  PropertyHolderSubclassDecl.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-10-04.
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