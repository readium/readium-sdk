//
//  Streams.cpp
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

#include "Streams.h"
#include <ppltasks.h>
#include <wrl/client.h>
#include <robuffer.h>

using namespace ::concurrency;
using namespace ::Microsoft::WRL;
using namespace ::Windows::Storage::Streams;

static ComPtr<IBufferByteAccess> getByteAccessForBuffer(IBuffer^ buffer)
{
	ComPtr<IUnknown> comBuffer(reinterpret_cast<IUnknown*>(buffer));
	ComPtr<IBufferByteAccess> byteBuffer;
	comBuffer.As(&byteBuffer);
	return byteBuffer;
}

BEGIN_READIUM_API

Stream::Stream(NativePtr native) : _native(native)
{
	if (!bool(_native))
		throw ref new ::Platform::InvalidArgumentException(TEXT("null native stream supplied"));
}
IAsyncOperationWithProgress<IBuffer^, unsigned int>^ Stream::ReadAsync(IBuffer^ buffer, unsigned int count, InputStreamOptions options)
{
	return create_async([this, buffer, count, options](progress_reporter<unsigned int> reporter) -> IBuffer^ {
		if (!IsOpen)
			return buffer;
		auto byteBuffer = getByteAccessForBuffer(buffer);
		byte* bytes = nullptr;
		byteBuffer->Buffer(&bytes);

		unsigned int toRead = ((options == InputStreamOptions::ReadAhead) ? std::max(count, buffer->Capacity) : std::min(count, buffer->Capacity));
		ssize_t numRead = 0;
		int total = 0;

		auto asyncPtr = std::dynamic_pointer_cast<::ePub3::AsyncByteStream>(_native);
		if (bool(asyncPtr))
		{
			::ePub3::AsyncEvent evt;
			do
			{
				evt = asyncPtr->WaitNextEvent();

			} while (evt == ::ePub3::AsyncEvent::HasSpaceAvailable || evt == ::ePub3::AsyncEvent::None);

			if (evt != ::ePub3::AsyncEvent::HasBytesAvailable)
				return buffer;
		}

		do
		{
			numRead = static_cast<ssize_t>(_native->ReadBytes(bytes, toRead));
			if (numRead < 0)
				break;

			total += numRead;
			toRead -= numRead;
			bytes += numRead;

			reporter.report(total);

		} while (numRead > 0 && toRead > 0 && options != InputStreamOptions::Partial);

		buffer->Length = total;
		return buffer;
	});
}
IAsyncOperation<bool>^ Stream::FlushAsync()
{
	return create_async([this]() -> bool {
		if (!IsOpen)
			return false;
		_native->Close();
		return true;
	});
}
IAsyncOperationWithProgress<unsigned int, unsigned int>^ Stream::WriteAsync(IBuffer^ buffer)
{
	return create_async([this, buffer](progress_reporter<unsigned int> reporter) -> unsigned int {
		if (!IsOpen)
			return 0;

		auto byteBuffer = getByteAccessForBuffer(buffer);
		byte* bytes = nullptr;
		byteBuffer->Buffer(&bytes);

		unsigned int toWrite = buffer->Length;
		ssize_t numWritten = 0;
		unsigned int total = 0;

		auto asyncPtr = std::dynamic_pointer_cast<::ePub3::AsyncByteStream>(_native);
		if (bool(asyncPtr))
		{
			::ePub3::AsyncEvent evt;
			do
			{
				evt = asyncPtr->WaitNextEvent();

			} while (evt == ::ePub3::AsyncEvent::HasBytesAvailable || evt == ::ePub3::AsyncEvent::None);

			if (evt != ::ePub3::AsyncEvent::HasSpaceAvailable)
				return 0;
		}

		do
		{
			numWritten = static_cast<ssize_t>(_native->WriteBytes(bytes, toWrite));
			if (numWritten < 0)
				break;

			total += numWritten;
			toWrite -= numWritten;
			bytes += numWritten;

			reporter.report(total);

		} while (numWritten > 0 && toWrite > 0);

		return total;
	});
}

RandomAccessStream::RandomAccessStream(NativePtr native) : _native(native)
{
	if (!bool(_native))
		throw ref new ::Platform::InvalidArgumentException(TEXT("null native stream supplied"));
}
IAsyncOperationWithProgress<IBuffer^, unsigned int>^ RandomAccessStream::ReadAsync(IBuffer^ buffer, unsigned int count, InputStreamOptions options)
{
	return create_async([this, buffer, count, options](progress_reporter<unsigned int> reporter) -> IBuffer^ {
		if (!IsOpen)
			return buffer;

		auto byteBuffer = getByteAccessForBuffer(buffer);
		byte* bytes = nullptr;
		byteBuffer->Buffer(&bytes);

		unsigned int toRead = ((options == InputStreamOptions::ReadAhead) ? std::max(count, buffer->Capacity) : std::min(count, buffer->Capacity));
		ssize_t numRead = 0;
		int total = 0;

		auto asyncPtr = std::dynamic_pointer_cast<::ePub3::AsyncByteStream>(_native);
		if (bool(asyncPtr))
		{
			::ePub3::AsyncEvent evt;
			do
			{
				evt = asyncPtr->WaitNextEvent();

			} while (evt == ::ePub3::AsyncEvent::HasSpaceAvailable || evt == ::ePub3::AsyncEvent::None);

			if (evt != ::ePub3::AsyncEvent::HasBytesAvailable)
				return buffer;
		}

		do
		{
			numRead = static_cast<ssize_t>(_native->ReadBytes(bytes, toRead));
			if (numRead < 0)
				break;

			total += numRead;
			toRead -= numRead;
			bytes += numRead;

			reporter.report(total);

		} while (numRead > 0 && toRead > 0 && options != InputStreamOptions::Partial);

		buffer->Length = total;
		return buffer;
	});
}
IAsyncOperation<bool>^ RandomAccessStream::FlushAsync()
{
	return create_async([this]() -> bool {
		if (!IsOpen)
			return false;
		_native->Close();
		return true;
	});
}
IAsyncOperationWithProgress<unsigned int, unsigned int>^ RandomAccessStream::WriteAsync(IBuffer^ buffer)
{
	return create_async([this, buffer](progress_reporter<unsigned int> reporter) -> unsigned int {
		if (!IsOpen)
			return 0;

		auto byteBuffer = getByteAccessForBuffer(buffer);
		byte* bytes = nullptr;
		byteBuffer->Buffer(&bytes);

		unsigned int toWrite = buffer->Length;
		ssize_t numWritten = 0;
		unsigned int total = 0;

		auto asyncPtr = std::dynamic_pointer_cast<::ePub3::AsyncByteStream>(_native);
		if (bool(asyncPtr))
		{
			::ePub3::AsyncEvent evt;
			do
			{
				evt = asyncPtr->WaitNextEvent();

			} while (evt == ::ePub3::AsyncEvent::HasBytesAvailable || evt == ::ePub3::AsyncEvent::None);

			if (evt != ::ePub3::AsyncEvent::HasSpaceAvailable)
				return 0;
		}

		do
		{
			numWritten = static_cast<ssize_t>(_native->WriteBytes(bytes, toWrite));
			if (numWritten < 0)
				break;

			total += numWritten;
			toWrite -= numWritten;
			bytes += numWritten;

			reporter.report(total);

		} while (numWritten > 0 && toWrite > 0);

		return total;
	});
}
unsigned long long RandomAccessStream::Position::get()
{
	if (!IsOpen)
		return 0;
	return _native->Position();
}
IRandomAccessStream^ RandomAccessStream::CloneStream()
{;
	if (!bool(_native))
		return nullptr;

	NativePtr newNative = _native->Clone();

	// creation might have failed due to closed file, etc.
	if (!bool(newNative))
		return nullptr;

	return ref new RandomAccessStream(newNative);
}
IInputStream^ RandomAccessStream::GetInputStreamAt(unsigned long long position)
{
	if (!bool(_native))
		return nullptr;

	IRandomAccessStream^ result = CloneStream();
	if (result == nullptr)
		return nullptr;

	// special-case for zip streams-- have to read data to reach a given position
	if (bool(std::dynamic_pointer_cast<::ePub3::ZipFileByteStream>(_native)))
	{
		RandomAccessStream^ _new = dynamic_cast<RandomAccessStream^>(result);
		
		std::shared_ptr<::ePub3::ZipFileByteStream> _stream = std::dynamic_pointer_cast<::ePub3::ZipFileByteStream>(_new->_native);
		size_t toSkip = static_cast<size_t>(position);
		ssize_t numRead = 0;
		do
		{
			byte buf[4096];
			_stream->ReadBytes(buf, std::min(toSkip, (size_t)4096));

		} while (toSkip > 0 && numRead != 0);
	}
	else
	{
		// file streams can seek natively
		result->Seek(position);
	}

	return result;
}
IOutputStream^ RandomAccessStream::GetOutputStreamAt(unsigned long long position)
{
	// can't generate an in-the-middle output stream for a zip file...
	if (!bool(_native) || bool(std::dynamic_pointer_cast<::ePub3::ZipFileByteStream>(_native)))
		return nullptr;

	IRandomAccessStream^ result = CloneStream();
	if (result == nullptr)
		return nullptr;

	result->Seek(position);
	return result;
}
void RandomAccessStream::Seek(unsigned long long position)
{
	_native->Seek(static_cast<::ePub3::ByteStream::size_type>(position), std::ios::beg);
}

END_READIUM_API
