//
//  Streams.h
//  Readium-WinRT
//
//  Created by Jim Dovey on 2013-09-25.
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
	using NativePtr = ::ePub3::ByteStreamPtr;
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
	using NativePtr = std::shared_ptr<::ePub3::SeekableByteStream>;
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
