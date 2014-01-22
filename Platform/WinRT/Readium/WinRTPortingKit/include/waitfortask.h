//
//  waitfortask.h
//  WinRTPortingKit
//
//  Created by Vincent Richomme on 2014-01-22.
//  Copyright (c) 2014-2013 The Readium Foundation and contributors.
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

#include <Windows.h>


template <typename T>
T wait_for_task(concurrency::task<T> task)
{
	bool exceptionCaptured = false;
	return wait_for_task(task, exceptionCaptured);
}

template <typename T>
T wait_for_task(concurrency::task<T> task, bool& exceptionCaptured)
{
	T result;

	DWORD waitResult = STATUS_PENDING;
	HANDLE hEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	//Récupération du Dispatcher
	auto window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	Windows::UI::Core::CoreDispatcher^ dispatcher = (window != nullptr) ? window->Dispatcher : nullptr;

	std::wstring exceptionMessage;
	if (dispatcher == nullptr)
	{
		task.then([&hEvent, &exceptionCaptured, &result](concurrency::task<T> previousTask){
			try
			{
				result = previousTask.get();
			}
			catch (...)
			{
				exceptionCaptured = true;
				result = nullptr;
			}
			SetEvent(hEvent);
		});
	}
	else
	{
		auto x = dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler(
			[&task, &hEvent, &exceptionCaptured, &result]() {
			try
			{
				result = task.get();
			}
			catch (Platform::Exception^ ex)
			{
				exceptionCaptured = true;
				result = nullptr;
				//exceptionMessage = std::wstring(ex->Message->Data());
			}
			catch (std::exception & ex)
			{
				exceptionCaptured = true;
				result = nullptr;
				//exceptionMessage = std::wstring(ex.what(), ex.what() + strlen(ex.what()));
			}
			catch (...)
			{
				exceptionCaptured = true;
				result = nullptr;
				//exceptionMessage = std::wstring(L"Unrecognized C++ exception.");
			}
			SetEvent(hEvent);
		}));
	}

	while (waitResult != WAIT_OBJECT_0)
	{
		if (dispatcher != nullptr)
			dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

		waitResult = ::WaitForSingleObjectEx(hEvent, 0, TRUE);
	}
	CloseHandle(hEvent);

	return result;
}



