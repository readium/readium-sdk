//
//  PropertyHolderSubclassDecl.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-10-04.
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

// Include this inside the declaration of a class which should implement IPropertyHolder
// I'm using this as a cut-and-paste method of writing a crapload of code

protected private:
	property ::ePub3::PropertyHolderPtr Native {
		::ePub3::PropertyHolderPtr get() {
				return std::dynamic_pointer_cast<::ePub3::PropertyHolder>(NativeObject);
		}
	}

public:
	property unsigned int Count { virtual unsigned int get(); }

	virtual void Append(Property^ prop);
	virtual void Append(IPropertyHolder^ allProps);

	virtual void Remove(Uri^ propertyIRI);
	virtual void Remove(String^ reference);
	virtual void Remove(String^ prefix, String^ reference);

	virtual Property^ At(unsigned int idx);
	virtual void EraseAt(unsigned int idx);

	virtual bool Contains(DCType type);
	virtual bool Contains(Uri^ propertyIRI);
	virtual bool Contains(String^ reference);
	virtual bool Contains(String^ prefix, String^ reference);

	virtual Property^ PropertyMatching(DCType type);
	virtual Property^ PropertyMatching(Uri^ propertyIRI);
	virtual Property^ PropertyMatching(String^ reference);
	virtual Property^ PropertyMatching(String^ prefix, String^ reference);

	virtual IVectorView<Property^>^ PropertiesMatching(DCType type);
	virtual IVectorView<Property^>^ PropertiesMatching(Uri^ propertyIRI);
	virtual IVectorView<Property^>^ PropertiesMatching(String^ reference);
	virtual IVectorView<Property^>^ PropertiesMatching(String^ prefix, String^ reference);

	virtual void RegisterPrefixIRIStem(String^ prefix, String^ iriStem);

	virtual Uri^ MakePropertyIRI(String^ reference);
	virtual Uri^ MakePropertyIRI(String^ prefix, String^ reference);
	virtual Uri^ PropertyIRIFromString(String^ str);

	virtual IIterator<Property^>^ First();
