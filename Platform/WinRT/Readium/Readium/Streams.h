//
//  Streams.h
//  Readium-WinRT
//
//  Created by Jim Dovey on 2013-09-25.
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

#ifndef __Readium_Streams_h__
#define __Readium_Streams_h__

#include "Readium.h"
#include <ePub3/utilities/byte_stream.h>
#include <robuffer.h>

using ::Platform::String;
//using ::Windows::Storage;
using ::Windows::Storage::Streams::IInputStream;
using ::Windows::Storage::Streams::IOutputStream;
using ::Windows::Storage::Streams::IRandomAccessStream;
using ::Windows::Storage::Streams::IBuffer;
using ::Windows::Storage::Streams::IBufferByteAccess;
using ::Windows::Foundation::IAsyncAction;
using ::Windows::Foundation::IAsyncOperation;
using ::Windows::Foundation::IAsyncOperationWithProgress;

BEGIN_READIUM_API

// the actual exported types are these two interfaces
public interface class IClosableStream : IInputStream, IOutputStream
{
public:
	void Close();
	property bool IsOpen { bool get(); }
};
public interface class IClosableRandomAccessStream : IClosableStream, IRandomAccessStream
{
};

// The implementation classes are internal
[Windows::Foundation::Metadata::WebHostHidden]
ref class Stream: public IClosableStream
{
private:
	typedef ::ePub3::ByteStreamPtr NativePtr;
	NativePtr	_native;

internal:
	Stream(NativePtr native);

public:
	virtual ~Stream() {}

public:

	// IClosableStream
	virtual void Close() { 
		if (IsOpen)
			_native->Close();
	}
	property bool IsOpen
	{
		virtual bool get() {
			if (bool(_native))
				return _native->IsOpen();
			return false;
		}
	}

	// IInputStream

	virtual IAsyncOperationWithProgress<IBuffer^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int count, ::Windows::Storage::Streams::InputStreamOptions options);

	// IOutputStream

	virtual IAsyncOperation<bool>^ FlushAsync();
	virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buffer);

};

ref class RandomAccessStream: public IClosableRandomAccessStream
{
private:
	typedef std::shared_ptr<::ePub3::SeekableByteStream> NativePtr;
	NativePtr	_native;

internal:
	RandomAccessStream(NativePtr native);	// checks to ensure it's actually random-accessible

public:
	virtual ~RandomAccessStream() {}

public:

	// IClosableStream

	virtual void Close() {
		if (IsOpen)
			_native->Close();
	}
	property bool IsOpen
	{
		virtual bool get() {
			if (bool(_native))
				return _native->IsOpen();
			return false;
		}
	}

	// IInputStream

	virtual IAsyncOperationWithProgress<IBuffer^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int count, ::Windows::Storage::Streams::InputStreamOptions options);

	// IOutputStream

	virtual IAsyncOperation<bool>^ FlushAsync();
	virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buffer);

	// IRandomAccessStream

	property bool CanRead
	{
		virtual bool get() {
			return (IsOpen ? _native->BytesAvailable() != 0 : false);
		}
	}
	property bool CanWrite
	{
		virtual bool get() {
			return (IsOpen ? _native->SpaceAvailable() != 0 : false);
		}
	}
	property unsigned long long Position
	{
		virtual unsigned long long get();
	}
	property unsigned long long Size
	{
		virtual unsigned long long get() { 
			return IsOpen ? Position + _native->BytesAvailable() : 0ULL;
		}
		virtual void set(unsigned long long) { /* nothing */ }
	}

	virtual IRandomAccessStream^ CloneStream();
	virtual IInputStream^ GetInputStreamAt(unsigned long long position);
	virtual IOutputStream^ GetOutputStreamAt(unsigned long long position);
	virtual void Seek(unsigned long long position);

};

END_READIUM_API

#endif	/* __Readium_Streams_h__ */