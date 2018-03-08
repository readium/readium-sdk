//
//  WinContentModuleManager.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once

#include "Readium.h"
#include "IContentModule.h"
#include "WinCredentialRequest.h"
#include "WinContainer.h"
#include "ExceptionWrapper.h"
#include <ePub3/content_module.h>

BEGIN_READIUM_API

template <typename _Result, typename _NativeResult = _Result>
static void __process_operation_completion(::Windows::Foundation::IAsyncOperation<_Result>^ operation, ::Windows::Foundation::AsyncStatus status, std::promise<_NativeResult>& promise,
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

	virtual
	std::future<bool>
	ApproveUserAction(const ::ePub3::UserAction& action)
		{
			using namespace ::Windows::Foundation;
			auto promise = new std::promise<bool>();
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
	std::future<ePub3::ContainerPtr> ProcessFileAsyncInternal(const ePub3::string& path)
		{
			using namespace ::Windows::Foundation;
			auto op = __winrt_->ProcessFile(StringFromNative(path));
			auto promise = new std::promise<::ePub3::ContainerPtr>();

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
		return create_async([__fut]() -> Container^ {
			return Container::Wrapper(__fut.get());
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
		return create_async([__fut]() -> bool {
			return __fut.get();
		});
	}

};

END_READIUM_API
