//
//  IContentHandler.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-02.
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

#ifndef __Readium_IContentHandler_h__
#define __Readium_IContentHandler_h__

#include "Readium.h"

BEGIN_READIUM_API

ref class Package;

using ::Platform::String;

public interface class IContentHandler
{
	property Package^ Owner { Package^ get(); }
	property String^ MediaType { String^ get(); }
	void Invoke(String^ packageRelativeSourcePath, ::Windows::Foundation::Collections::IMapView<String^, String^>^ parameters);
};

END_READIUM_API

#endif	/* __Readium_IContentHandler_h__ */
