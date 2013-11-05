//
//  WinErrorHandler.cpp
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

#include "WinErrorHandler.h"
#include <atomic>

BEGIN_READIUM_API

static ErrorHandler^	__default_error_handler = nullptr;
static ErrorHandling^	__error_handling_singleton = nullptr;

ErrorHandling::ErrorHandling()
{
	if (__error_handling_singleton != nullptr && __error_handling_singleton != this)
		throw ref new ::Platform::COMException(E_NOT_VALID_STATE, TEXT("The ErrorHandling singleton already exists!"));

	__default_error_handler = ref new ErrorHandler([](ErrorInfo^ error) {
		return ::ePub3::DefaultErrorHandler(error->CppError);
	});
}
ErrorHandling^ ErrorHandling::Instance()
{
	static std::atomic_flag __once_flag;
	if (__once_flag.test_and_set() == false)
	{
		__error_handling_singleton = ref new ErrorHandling();
	}
	return __error_handling_singleton;
}
ErrorHandler^ ErrorHandling::Default::get()
{
	return __default_error_handler;
}
ErrorHandler^ ErrorHandling::Current::get()
{
	auto nativeCurrent = ::ePub3::ErrorHandler();
	return ref new ErrorHandler([nativeCurrent](ErrorInfo^ error) {
		return nativeCurrent(error->CppError);
	});
}
void ErrorHandling::Current::set(ErrorHandler^ newHandler)
{
	if (newHandler == __default_error_handler)
		::ePub3::SetErrorHandler(::ePub3::DefaultErrorHandler);

	::ePub3::SetErrorHandler([newHandler](const ePub3::error_details& err) {
		return newHandler->Invoke(ref new ErrorInfo(err));
	});
}
void ErrorHandling::HandleError(EPUBError error)
{
	::ePub3::HandleError(::ePub3::EPUBError(error));
}
void ErrorHandling::HandleError(EPUBError error, ::Platform::String^ message)
{
	::ePub3::string str(StringToNative(message));
	::ePub3::HandleError(::ePub3::EPUBError(error), str.c_str());
}

::Platform::String^ ErrorInfo::Message::get()
{
	if (__message_cached_ == nullptr)
		__message_cached_ = StringFromCString(__error_.message());
	return __message_cached_;
}
::Platform::String^ ErrorInfo::Category::get()
{
	if (__category_cached_ == nullptr)
		__category_cached_ = StringFromCString(__error_.category().name());
	return __category_cached_;
}

END_READIUM_API
