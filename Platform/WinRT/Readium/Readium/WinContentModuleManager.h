//
//  WinContentModuleManager.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once

#include "Readium.h"
#include "IContentModule.h"
#include "WinCredentialRequest.h"
#include "WinContainer.h"
#include "ExceptionWrapper.h"
#include <ePub3/content_module.h>

#ifndef async_result
#define async_result ePub3::future
#endif

BEGIN_READIUM_API

template <typename _Result, typename _NativeResult = _Result>
//static void __process_operation_completion(::Windows::Foundation::IAsyncOperation<_Result>^ operation, ::Windows::Foundation::AsyncStatus status, std::promise<_NativeResult>& promise,
static void __process_operation_completion(::Windows::Foundation::IAsyncOperation<_Result>^ operation, ::Windows::Foundation::AsyncStatus status, ePub3::promise<_NativeResult>& promise,
	std::function<void()> process_result)
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
	std::future<ePub3::ContainerPtr>
	ProcessFile(const ::ePub3::string& path, std::launch policy = std::launch::async)
		{
			using namespace ::Windows::Foundation;

			if (policy == std::launch::deferred)
			{
				Container^ container = __winrt_->ProcessFileSync(StringFromNative(path));
				std::promise<ePub3::ContainerPtr> __p;
				if (container)
					__p.set_value(container->NativeObject);
				else
					__p.set_value(nullptr);
				return __p.get_future();
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

	//std::future<bool>
	virtual
	async_result<bool>
	ApproveUserAction(const ::ePub3::UserAction& action)
		{
			using namespace ::Windows::Foundation;
			//std::promise<bool> *promise = new std::promise<bool>();
			//auto promise = new std::promise<bool>();
			auto promise = new ePub3::promise<bool>();
			auto op = __winrt_->ApproveUserAction(ref new UserAction(action));

			op->Completed = ref new AsyncOperationCompletedHandler<bool>([promise](IAsyncOperation<bool>^ operation, AsyncStatus status) {
				__process_operation_completion(operation, status, *promise, [operation, promise]() {
					promise->set_value(operation->GetResults());
				});
				delete promise;
			});

			return promise->get_future();
		}

private:
	/*std::future<ePub3::ContainerPtr> ProcessFileAsyncInternal(const ePub3::string& path)*/
	ePub3::future<ePub3::ContainerPtr> ProcessFileAsyncInternal(const ePub3::string& path)
		{
			using namespace ::Windows::Foundation;
			auto op = __winrt_->ProcessFile(StringFromNative(path));
			//auto promise = new std::promise<::ePub3::ContainerPtr>();
			auto promise = new ePub3::promise<::ePub3::ContainerPtr>();

			op->Completed = ref new AsyncOperationCompletedHandler<Container^>([promise](IAsyncOperation<Container^>^ operation, AsyncStatus status) {
				__process_operation_completion(operation, status, *promise, [operation, promise]() {
					Container^ container = operation->GetResults();
					if (container == nullptr)
						promise->set_value(nullptr);
					else
						promise->set_value(container->NativeObject);
				});
				delete promise;
			});

			return promise->get_future();
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
		using namespace ::concurrency;
		auto __fut = _native->ProcessFile(StringToNative(path)).share();
		auto __nat = __fut.get();
		return create_async([__fut, __nat]() -> Container^ {
			/*return Container::Wrapper(__fut.get());*/
			return Container::Wrapper(__nat);
		});
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
		auto __fut = _native->ApproveUserAction(action->Native).share();
		auto tmp = __fut.get();

		return create_async([__fut, tmp]() -> bool {
			//return __fut.get();
			return tmp;
		});
	}

};

END_READIUM_API
