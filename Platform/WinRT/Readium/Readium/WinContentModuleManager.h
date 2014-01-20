//
//  WinContentModuleManager.h
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
#include "IContentModule.h"
#include "WinCredentialRequest.h"
#include "WinContainer.h"
#include "ExceptionWrapper.h"
#include <ePub3/content_module.h>

BEGIN_READIUM_API

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
// the async_result<> and promised_result<> types are actually types here;
// otherwise, they're macros, and cannot be used with a namespace.
// to keep the code working in both cases, we'll tell the compiler that the
// names refer to those in the 'ePub3' namespace when we're using actual types.
using ::ePub3::async_result;
using ::ePub3::promised_result;
#endif

template <
	typename _Result,
	typename _NativeResult
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULT_TEMPLATE_ARGS_ON_FUNCTIONS)
		= _Result
#endif
	>
static void __process_operation_completion(::Windows::Foundation::IAsyncOperation<_Result>^ operation, ::Windows::Foundation::AsyncStatus status, promised_result<_NativeResult>& promise,
	::std::function<void()> process_result)
{
	using namespace ::Windows::Foundation;
	if (status == AsyncStatus::Completed)
	{
		process_result();
	}
	else if (status == AsyncStatus::Error || operation->ErrorCode.Value != 0)
	{
		auto rt_exc = ref new ::Platform::COMException(operation->ErrorCode.Value);
		promise.set_exception(std::make_exception_ptr(__WinRTException(rt_exc)));
	}
	else if (status == AsyncStatus::Canceled)
	{
		promise.set_exception(std::make_exception_ptr(std::system_error(std::make_error_code(std::errc::operation_canceled))));
	}
	else
	{
		promise.set_exception(std::make_exception_ptr(std::system_error(std::make_error_code(std::errc::state_not_recoverable))));
	}
}

#if !EPUB_COMPILER_SUPPORTS(CXX_DEFAULT_TEMPLATE_ARGS_ON_FUNCTIONS)
template <typename _Result>
static void __process_operation_completion(::Windows::Foundation::IAsyncOperation<_Result>^ operation, ::Windows::Foundation::AsyncStatus status, promised_result<_Result>& promise,
	::std::function<void()> process_result)
{
	__process_operation_completion<_Result, _Result>(operation, status, promise, process_result);
}
#endif

class __WinRTContentModule
	: public ::ePub3::ContentModule
{
private:
	IContentModule^	__winrt_;

public:
	__WinRTContentModule(IContentModule^ __rt)
		: __winrt_(__rt)
		{}

	virtual ~__WinRTContentModule()
		{}

	IContentModule^ __rtObj() const
		{
			return __winrt_;
		}

	virtual
	async_result<ePub3::ContainerPtr>
	ProcessFile(const ::ePub3::string& path, ePub3::launch policy = ePub3::launch::async)
		{
			using namespace ::Windows::Foundation;

			if (policy == ePub3::launch::deferred)
			{
				Container^ container = __winrt_->ProcessFileSync(StringFromNative(path));
				promised_result<ePub3::ContainerPtr> __p;
				if (container)
					__set_promise(__p, container->NativeObject);
				else
					__set_promise(__p, nullptr);
				return __ar_from_promise(__p, ePub3::ContainerPtr);
			}
			else
			{
				return ProcessFileAsyncInternal(path);
			}
		}

	//////////////////////////////////////////////
	// Content Filters

	virtual
	void
	RegisterContentFilters()
		{
			__winrt_->RegisterContentFilters();
		}

	//////////////////////////////////////////////
	// User actions

	virtual
	async_result<bool>
	ApproveUserAction(const ::ePub3::UserAction& action)
		{
			using namespace ::Windows::Foundation;
			auto promise = new promised_result<bool>();
			auto op = __winrt_->ApproveUserAction(ref new UserAction(action));

			op->Completed = ref new AsyncOperationCompletedHandler<bool>([promise](IAsyncOperation<bool>^ operation, AsyncStatus status) {
				__process_operation_completion(operation, status, *promise, [operation, promise]() {
					__set_promise(*promise, operation->GetResults());
				});
				delete promise;
			});
			return __ar_from_promise(*promise, bool);
		}

private:
	async_result<ePub3::ContainerPtr> ProcessFileAsyncInternal(const ePub3::string& path)
		{
			using namespace ::Windows::Foundation;
			auto op = __winrt_->ProcessFile(StringFromNative(path));
			auto promise = new promised_result<::ePub3::ContainerPtr>();

			op->Completed = ref new AsyncOperationCompletedHandler<Container^>([promise](IAsyncOperation<Container^>^ operation, AsyncStatus status) {
				__process_operation_completion(operation, status, *promise, [operation, promise]() {
					Container^ container = operation->GetResults();
					if (container == nullptr)
						__set_promise(*promise, nullptr);
					else
						__set_promise(*promise, container->NativeObject);
				});
				delete promise;
			});

			return __ar_from_promise(*promise, ePub3::ContainerPtr);
		}

};

public ref class ContentModuleManager sealed
{
private:
	ContentModuleManager() {}

public:
	virtual ~ContentModuleManager() {}

	static void RegisterContentModule(IContentModule^ module, ::Platform::String^ name);

	static void DisplayMessage(String^ title, String^ message);
	static ::Windows::Foundation::IAsyncOperation<Credentials^>^ RequestCredentialInput(CredentialRequest^ request);

};

ref class ContentModuleWrapper : IContentModule
{
private:
	std::shared_ptr<ePub3::ContentModule>	_native;

internal:
	ContentModuleWrapper(std::shared_ptr<ePub3::ContentModule> native)
		: _native(native) {}

public:
	virtual ~ContentModuleWrapper() {}

	virtual ::Windows::Foundation::IAsyncOperation<Container^>^ ProcessFile(::Platform::String^ path)
	{
#if EPUB_PLATFORM(WIN_PHONE)
		using namespace ::concurrency;
		auto __task = _native->ProcessFile(StringToNative(path));

		// fiddly things to return the right type-- can't just pass a task in, have to pass a lambda which returns the task...
		return create_async([__task]() {
			return __task.then([](ePub3::ContainerPtr native) {
				return Container::Wrapper(native);
			});
		});
#else
		using namespace ::concurrency;
		auto __fut = _native->ProcessFile(StringToNative(path)).share();
		return create_async([__fut]() -> Container^ {
			auto shared = __fut;	// MSVC complains that __fut is const if I call __fut.get(). Sigh.
			return Container::Wrapper(shared.get());
		});
#endif
	}

	virtual Container^ ProcessFileSync(::Platform::String^ path)
	{
		auto __fut = _native->ProcessFile(StringToNative(path));
		auto __nat = __fut.get();
		if (!bool(__nat))
			return nullptr;
		return Container::Wrapper(__nat);
	}

	virtual void RegisterContentFilters()
	{
		_native->RegisterContentFilters();
	}

	virtual ::Windows::Foundation::IAsyncOperation<bool>^ ApproveUserAction(UserAction^ action)
	{
		using namespace ::concurrency;
#if EPUB_PLATFORM(WIN_PHONE)
		return create_async([this, action]() {
			return _native->ApproveUserAction(action->Native);
		});
#else
		auto __fut = _native->ApproveUserAction(action->Native).share();
		return create_async([__fut]() -> bool {
			auto shared = __fut;		// no idea why MSVC thinks __fut is const when calling its members...
			return shared.get();
		});
#endif
	}

};

END_READIUM_API
