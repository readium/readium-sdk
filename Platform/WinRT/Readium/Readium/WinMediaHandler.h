//
//  WinMediaHandler.h
//  Readium
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

#ifndef __Readium_MediaHandler_h__
#define __Readium_MediaHandler_h__

#include "Readium.h"
#include "IContentHandler.h"
#include <ePub3/content_handler.h>

BEGIN_READIUM_API

public ref class MediaHandler sealed : IContentHandler
{
	_DECLARE_BRIDGE_API_(::ePub3::MediaHandlerPtr, MediaHandler^);

internal:
	MediaHandler(::ePub3::MediaHandlerPtr native);

public:
	virtual ~MediaHandler() {}

	// IContentHandler interface

	property Package^ Owner { virtual Package^ get(); }
	property String^ MediaType { virtual String^ get(); }
	virtual void Invoke(String^ packageRelativeSourcePath, ::Windows::Foundation::Collections::IMapView<String^, String^>^ parameters);

	// MediaHandler extras

	::Windows::Foundation::Uri^ Target(String^ packageRelativeSourcePath, ::Windows::Foundation::Collections::IMapView<String^, String^>^ parameters);

};

END_READIUM_API

#endif	/* __Readium_MediaHandler_h__ */
