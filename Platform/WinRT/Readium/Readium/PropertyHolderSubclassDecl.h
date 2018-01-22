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
