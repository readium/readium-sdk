//
//  ExceptionWrapper.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once
#include "Readium.h"
#include <exception>

BEGIN_READIUM_API

class __WinRTException : public std::exception
{
private:
	typedef std::wstring_convert<std::codecvt_utf8<wchar_t>>	UTF8Converter;

public:
	__WinRTException(::Platform::Exception^ exc)
		: std::exception(UTF8Converter().to_bytes(exc->Message->Data(), exc->Message->Data ()+ exc->Message->Length()).c_str(), exc->HResult)
	{}

};

END_READIUM_API
