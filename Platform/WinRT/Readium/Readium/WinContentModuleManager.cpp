//
//  WinContentModuleManager.cpp
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

#include "WinContentModuleManager.h"
#include <ePub3/content_module_manager.h>
#include <ePub3/content_module.h>
#include "WinContainer.h"
#include "ExceptionWrapper.h"
#include "CollectionBridges.h"

#include <ppltasks.h>

using namespace ::Platform;
using namespace ::Windows::Foundation;

BEGIN_READIUM_API

template <typename _Result, typename _NativeResult = _Result>
static void __process_operation_completion(IAsyncOperation<_Result>^ operation, AsyncStatus status, std::promise<_NativeResult>& promise,
	std::function<void()> process_result)
{
	if (status == AsyncStatus::Completed)
	{
		process_result();
	}
	else if (status == AsyncStatus::Error || operation->ErrorCode.Value != 0)
	{
		auto rt_exc = ref new COMException(operation->ErrorCode.Value);
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

	virtual
	std::future<ePub3::ContainerPtr>
	ProcessFile(const ::ePub3::string& path, std::launch policy = std::launch::any)
		{
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

};

void ContentModuleManager::RegisterContentModule(IContentModule^ module, String^ name)
{
	::ePub3::ContentModuleManager::Instance()->RegisterContentModule(std::make_unique<__WinRTContentModule>(module), StringToNative(name));
}

void ContentModuleManager::DisplayMessage(String^ title, String^ message)
{
	::ePub3::ContentModuleManager::Instance()->DisplayMessage(StringToNative(title), StringToNative(message));
}
IAsyncOperation<Credentials^>^ ContentModuleManager::RequestCredentialInput(CredentialRequest^ request)
{
	std::future<::ePub3::Credentials> future = ::ePub3::ContentModuleManager::Instance()->RequestCredentialInput(request->NativeObject);
	auto shared = future.share();

	return ::concurrency::create_async([shared]() -> Credentials^ {
		return ref new BridgedStringToStringMapView(shared.get());
	});
}

END_READIUM_API
