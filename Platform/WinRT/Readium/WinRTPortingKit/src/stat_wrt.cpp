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

#include "stdafx.h"
#include "wrtcompatibility.h"

#define NOMINMAX

// DEBUGGING
#include <tchar.h>
#include <atltrace.h>

#if POSIX_WRT
#include <algorithm>
#include <ppl.h>
#include <ppltasks.h>
#include <robuffer.h>
#include "waitfortask.h"



using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Streams;

using namespace ::Microsoft::WRL;
using Windows::UI::Core::CoreDispatcher;
using Windows::UI::Core::CoreDispatcherPriority;
using Windows::UI::Core::CoreWindow;
using Windows::UI::Core::DispatchedHandler;

using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IAsyncOperation;

//#include <windows.storage.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

static int _hasRetrievedDefaultPath = 0;

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

std::string ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

Platform::String^ GetFutureAccesTokenFromPath(const char * path)
{
	auto itEntries = StorageApplicationPermissions::FutureAccessList->Entries;
	for (unsigned int i = 0; i < itEntries->Size; i++)
	{
		auto facl = itEntries->GetAt(i);
		std::string metadata = ws2s(std::wstring(facl.Metadata->Data(), facl.Metadata->Length()));
		if (strcmp(metadata.c_str(), path) == 0)
		{
			return facl.Token;
		}
	}

	return nullptr;
}


StorageFile^ GetStorageFileFromPath(const char * _Path)
{
	StorageFile^ storageFile = nullptr;

	if (_Path != nullptr)
	{
		Platform::String^ futureAccesToken = GetFutureAccesTokenFromPath(_Path);
		if (futureAccesToken != nullptr)
		{
			storageFile = wait_for_task(create_task(StorageApplicationPermissions::FutureAccessList->GetFileAsync(futureAccesToken)));
		}
		else
		{
			storageFile = wait_for_task(create_task(StorageFile::GetFileFromPathAsync(ref new Platform::String(s2ws(_Path).c_str()))));
		}
	}

	return storageFile;
}

int __cdecl stat_wrt(const char * _Path, struct stat * _Stat)
{
	int retCode = -1;

	if (_Path != nullptr)
	{
		StorageFile^ storageFile = GetStorageFileFromPath(_Path);
		if (storageFile != nullptr)
		{
			bool exceptionRaised = false;
			auto getFilePropsTask = create_task(storageFile->GetBasicPropertiesAsync());
			FileProperties::BasicProperties^ fileProps = wait_for_task(getFilePropsTask, exceptionRaised);
			if (fileProps != nullptr && exceptionRaised == false)
			{
				retCode = 0;
				if (_Stat != nullptr)
				{
					_Stat->st_size = (long)fileProps->Size;
				}
			}	
		}
		else
		{
			//FIXME
			retCode = -2;
		}

		
	}

	ATLTRACE(L"** stat_wrt returns %i : size=%d\n", retCode, _Stat->st_size);
	return retCode;
}


FILE * __cdecl fopen_wrt(const char * _Path, const char * _Mode)
{
	FILE* pIoBuff = nullptr;

	if (_Path != nullptr)
	{
		StorageFile^ storageFile = GetStorageFileFromPath(_Path);
		if (storageFile != nullptr)
		{
#if 1
			bool exceptionRaised = false;
			auto readStream = wait_for_task(create_task(storageFile->OpenAsync(FileAccessMode::Read)), exceptionRaised);
			if (readStream != nullptr)
			{
				IInspectable* pIRandomAccessStream = reinterpret_cast<IInspectable*>(readStream);
				pIRandomAccessStream->AddRef();

				DataReader^ dataReader = ref new DataReader(readStream);
				IInspectable* pDataReader = reinterpret_cast<IInspectable*>(dataReader);
				pDataReader->AddRef();

				pIoBuff = (FILE*)calloc(1, sizeof(FILE));
				pIoBuff->_ptr = (char*)pIRandomAccessStream;
				pIoBuff->_base = (char*)pDataReader;
			}
#else
			try
			{
				create_task(storageFile->OpenAsync(FileAccessMode::Read))
					.then([&pIoBuff](task<IRandomAccessStream^> task) {

					IRandomAccessStream^ readStream = task.get();
					IInspectable* pIRandomAccessStream = reinterpret_cast<IInspectable*>(readStream);
					pIRandomAccessStream->AddRef();

					DataReader^ dataReader = ref new DataReader(readStream);
					IInspectable* pDataReader = reinterpret_cast<IInspectable*>(dataReader);
					pDataReader->AddRef();

					pIoBuff = (FILE*)calloc(1, sizeof(FILE));
					pIoBuff->_ptr = (char*)pIRandomAccessStream;
					pIoBuff->_base = (char*)pDataReader;
				}).wait();

			}
			catch (...)
			{
				pIoBuff = nullptr;
			}
#endif
		}
	}

	ATLTRACE(L"** fopen_wrt returns 0x%p\n", pIoBuff);
	return pIoBuff;
}


int __cdecl fseek_wrt(FILE * _File, long _Offset, int _Origin)
{
	int ret = 0;

	if (_File != nullptr)
	{
		Streams::IRandomAccessStream^ stream = reinterpret_cast<Streams::IRandomAccessStream^>((IInspectable*)_File->_ptr);
		if (stream != nullptr)
		{
			unsigned long long position;
			if (_Origin == SEEK_CUR)
				position = stream->Position + _Offset;
			else if (_Origin == SEEK_END)
				position = std::min(stream->Size, stream->Size + _Offset);
			else if (_Origin == SEEK_SET)
				position = 0 + _Offset;

			try
			{
				stream->Seek(position);
				ret = 0;
			}
			catch (...)
			{
				ret = -1;
			}
		}
	}

	long curPos = ftell_wrt(_File);
	ATLTRACE(L"** fseek_wrt(0x%p, %d, %d) returns %i : curPos=%d\n", _File, _Offset, _Origin, ret, curPos);
	return ret;
}

long __cdecl ftell_wrt(FILE * _File)
{
	long position = -1;

	if (_File != nullptr)
	{
		Streams::IRandomAccessStream^ stream = reinterpret_cast<Streams::IRandomAccessStream^>((IInspectable*)_File->_ptr);
		if (stream != nullptr)
		{
			position = (long)stream->Position;
		}
	}

	ATLTRACE(L"** ftell_wrt returns %d\n", position);
	return position;
}


//inline Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> GetBufferByteAccess(Windows::Storage::Streams::IBuffer^ buffer){
//	return com_interface<Windows::Storage::Streams::IBufferByteAccess>(buffer);
//}

byte* ToNativeByteArray(IBuffer^ pixelBuffer, unsigned int *length)
{
	if (length != nullptr)
	{
		*length = pixelBuffer->Length;
	}
	// Query the IBufferByteAccess interface.
	ComPtr<IBufferByteAccess> bufferByteAccess;
	reinterpret_cast<IInspectable*>(pixelBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

	// Retrieve the buffer data.
	byte* data = nullptr;
	bufferByteAccess->Buffer(&data);
	return data;
}

size_t __cdecl fread_wrt(void * _Buffer, size_t _ElementSize, size_t _Count, FILE * _File)
{
	unsigned int totalRead = 0;

	if (_File != nullptr)
	{
		try
		{
			unsigned int size = _ElementSize * _Count;

#if 1
			Streams::IRandomAccessStream^ stream = reinterpret_cast<Streams::IRandomAccessStream^>((IInspectable*)_File->_ptr);
			
			long curPos = stream->Position;
			IBuffer^ buffer = ref new Buffer(size);
			create_task(stream->ReadAsync(buffer, size, InputStreamOptions::None)).then([&stream, &curPos, &_Buffer, &totalRead](IBuffer^ buffer)
			{
				byte* data = ToNativeByteArray(buffer, &totalRead);
				memcpy(_Buffer, data, totalRead);
				stream->Seek(curPos);
			}).wait();
#else
			DataReader^ dataReader = reinterpret_cast<DataReader^>((IInspectable*)_File->_base);
			create_task(dataReader->LoadAsync(size)).then([&dataReader, &totalRead, &_Buffer](unsigned int numBytesLoaded)
			{
				Platform::WriteOnlyArray<unsigned char, 1U>^ bufferArray = ref new Platform::Array<unsigned char, 1U>(numBytesLoaded);
				dataReader->ReadBytes(bufferArray);
				memcpy(_Buffer, bufferArray->begin(), bufferArray->end() - bufferArray->begin());
				totalRead = numBytesLoaded;

			}).wait(); /* block with wait since we're in a worker thread */
#endif
		}
		catch (int ex){
			ATLTRACE(L"Exception in read\n");
		}

	}

	long curPos = ftell_wrt(_File);
	ATLTRACE(L"** fread_wrt returns %d : curPos=%d\n", totalRead, curPos);
	return totalRead;
}

int __cdecl fclose_wrt(FILE * _File)
{
	if (_File != nullptr)
	{
		if (_File->_base != nullptr)
		{
			IInspectable* pDataReader = reinterpret_cast<IInspectable*>(_File->_base);
			pDataReader->Release();
		}
		if (_File->_ptr != nullptr)
		{
			IInspectable* pIRandomAccessStream = reinterpret_cast<IInspectable*>(_File->_ptr);
			pIRandomAccessStream->Release();
		}
		free(_File);
	}

	ATLTRACE(L"fclose_wrt returns %d\n", 0);
	return 0;
}

void __cdecl clearerr_wrt(FILE * _File) 
{ 
	ATLTRACE(L"** clearerr_wrt returns\n");
}

int __cdecl feof_wrt(_In_ FILE * _File)
{
	int isEof = 0;

	if (_File != nullptr)
	{
		try
		{
			Streams::IRandomAccessStream^ readStream = reinterpret_cast<Streams::IRandomAccessStream^>((IInspectable*)_File->_ptr);
			isEof = readStream->Position >= readStream->Size;
		}
		catch (int ex){
			OutputDebugString(L"Exception in read\n");
		}
	}

	ATLTRACE(L"** feof_wrt returns %i\n", isEof);
	return isEof;
}

int __cdecl ferror_wrt(_In_ FILE * _File)
{
	ATLTRACE(L"** ferror_wrt returns %i\n", 0);
	return 0;
}


#else

#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int __cdecl stat_wrt(const char * _StorageFile, struct stat * _Stat) 
{ 
	int ret = stat(_StorageFile, _Stat);
	ATLTRACE(L"** stat_wrt returns %i : size=%d\n", ret, _Stat->st_size);
	return  ret;
}

FILE * __cdecl fopen_wrt(const char * _Filename, const char * _Mode) 
{ 
	FILE * file = fopen(_Filename, _Mode);
	ATLTRACE(L"** fopen returns 0x%p\n", file);
	return  file;
}

int __cdecl fseek_wrt(FILE * _File, long _Offset, int _Origin) 
{ 
	int ret = fseek(_File, _Offset, _Origin); 

	long curPos = ftell_wrt(_File);
	ATLTRACE(L"** fseek(0x%p, %d, %d) returns %i : curPos=%d\n", _File, _Offset, _Origin, ret, curPos);
	return ret;
}

long __cdecl ftell_wrt(FILE * _File) 
{ 
	long ret = ftell(_File);
	ATLTRACE(L"** ftell returns %d\n", ret);
	return ret;
}

size_t __cdecl fread_wrt(void * buffer, size_t _ElementSize, size_t _Count, FILE * _File) 
{ 
	long curPos = ftell_wrt(_File);
	size_t ret = fread(buffer, _ElementSize, _Count, _File);
	ATLTRACE(L"** fread returns %d : curPos=%d\n", ret, curPos);
	return ret; 
}

int __cdecl fclose_wrt(FILE * _File) 
{ 
	int ret = fclose(_File);
	ATLTRACE(L"** fclose returns %i\n", ret);
	return ret; 
}

void __cdecl clearerr_wrt(FILE * _File) 
{ 
	clearerr(_File); 
	ATLTRACE(L"** clearerr returns\n");
}

int __cdecl feof_wrt(_In_ FILE * _File) 
{ 
	int ret = feof(_File); 
	ATLTRACE(L"** feof returns %d\n", ret);
	return ret; 
}

int __cdecl ferror_wrt(_In_ FILE * _File) 
{ 
	int ret = ferror(_File);
	ATLTRACE(L"** ferror returns %d\n", ret);
	return ret; 
}

#endif
