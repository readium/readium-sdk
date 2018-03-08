//
//  io_win.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-09-24.
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

#include "io.h"
#include <ePub3/xml/document.h>

#include <ppltasks.h>
#include <robuffer.h>
#include <wrl/client.h>
#include <functional>
#include <codecvt>
#include <sstream>

#include <MsXml6.h>

using namespace ::concurrency;
using namespace ::Windows::Data::Xml::Dom;
using namespace ::Windows::Storage;
using namespace ::Windows::Storage::Streams;
using namespace ::Windows::Foundation;
using namespace ::Platform;
using namespace ::Microsoft::WRL;

#define _UNEXPECTED_(x) std::cerr << "Unexpected method call: " << __PRETTY_FUNCTION__ << std::endl; return x

namespace Readium
{
	namespace XML
	{
		using ReadFn = std::function<int(void*, char*, int)>;
		using WriteFn = std::function<int(void*, const char*, int)>;
		using CloseFn = std::function<int(void*)>;

		static ::ComPtr<IBufferByteAccess> getByteAccessForBuffer(IBuffer^ buffer)
		{
			ComPtr<IUnknown> comBuffer(reinterpret_cast<IUnknown*>(buffer));
			ComPtr<IBufferByteAccess> byteBuffer;
			comBuffer.As(&byteBuffer);
			return byteBuffer;
		}

		ref class InputWrapper : public IInputStream
		{
		private:
			ReadFn _readCB;
			void* _ctx;

		internal:
			InputWrapper(ReadFn cb, void* ctx) : _readCB(cb), _ctx(ctx) {}

		public:
			virtual ~InputWrapper() {}

		public:
			virtual IAsyncOperationWithProgress<IBuffer^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int count, InputStreamOptions options) {
				return create_async([this, buffer, count, options](progress_reporter<unsigned int> reporter) -> IBuffer^ {
					auto byteBuffer = getByteAccessForBuffer(buffer);
					byte* bytes = nullptr;
					byteBuffer->Buffer(&bytes);

					unsigned int toRead = ((options == InputStreamOptions::ReadAhead) ? max(count, 4096) : count);
					ssize_t numRead = 0;
					int total = 0;

					do
					{
						numRead = _readCB(_ctx, reinterpret_cast<char*>(bytes), toRead);
						if (numRead < 0)
							break;

						total += numRead;
						toRead -= numRead;
						bytes += numRead;

						reporter.report(total);

					} while (numRead > 0 && toRead > 0 && options != InputStreamOptions::Partial);

					return buffer;
				});
			}

		};

		ref class RandomWrapper : public IRandomAccessStreamWithContentType
		{
		private:
			ReadFn _read;
			WriteFn _write;
			void* _ctx;

		internal:
			RandomWrapper(ReadFn reader, WriteFn writer, void* ctx) : _read(reader), _write(writer), _ctx(ctx) {}

		public:
			virtual ~RandomWrapper() {}

		public:
			property bool CanRead
			{
				virtual bool get() { return bool(_read); }
			}
			property bool CanWrite
			{
				virtual bool get() { return bool(_write); }
			}
			property unsigned long long Position
			{
				virtual unsigned long long get()
				{
					if (CanRead)
					{
						auto p = reinterpret_cast<::ePub3::xml::InputBuffer*>(_ctx);
						return p->offset();
					}
					else
					{
						auto p = reinterpret_cast<::ePub3::xml::OutputBuffer*>(_ctx);
						return p->offset();
					}
				}
			}
			property unsigned long long Size
			{
				virtual unsigned long long get()
				{
					if (CanRead)
					{
						auto p = reinterpret_cast<::ePub3::xml::InputBuffer*>(_ctx);
						return p->size();
					}
					else
					{
						auto p = reinterpret_cast<::ePub3::xml::OutputBuffer*>(_ctx);
						return p->size();
					}
				}
				virtual void set(unsigned long long s) {}
			}
			property String^ ContentType
			{
				virtual String^ get() { return TEXT("application/xml"); }
			}

			// bad idea
			virtual IRandomAccessStream^ CloneStream() { return ref new RandomWrapper(_read, _write, _ctx); }

			virtual IInputStream^ GetInputStreamAt(unsigned long long pos)
			{
				if (pos != Position)
					return nullptr;
				return this;
			}
			virtual IOutputStream^ GetOutputStreamAt(unsigned long long pos)
			{
				if (pos != Position)
					return nullptr;
				return this;
			}
			virtual void Seek(unsigned long long) {}

			virtual IAsyncOperationWithProgress<IBuffer^, unsigned int>^ ReadAsync(IBuffer^ buffer, unsigned int count, InputStreamOptions options) {
				return create_async([this, buffer, count, options](progress_reporter<unsigned int> reporter) -> IBuffer^ {
					auto byteBuffer = getByteAccessForBuffer(buffer);
					byte* bytes = nullptr;
					byteBuffer->Buffer(&bytes);

					unsigned int toRead = ((options == InputStreamOptions::ReadAhead) ? max(count, 4096) : count);
					ssize_t numRead = 0;
					unsigned int total = 0;

					do
					{
						numRead = _read(_ctx, reinterpret_cast<char*>(bytes), toRead);
						if (numRead < 0)
							break;

						total += numRead;
						toRead -= numRead;
						bytes += numRead;

						reporter.report(total);

					} while (numRead > 0 && toRead > 0 && options != InputStreamOptions::Partial);

					return buffer;
				});
			}
			virtual IAsyncOperationWithProgress<unsigned int, unsigned int>^ WriteAsync(IBuffer^ buffer)
			{
				return create_async([this, buffer](progress_reporter<unsigned int> reporter) -> unsigned int {
					auto byteBuffer = getByteAccessForBuffer(buffer);
					byte* bytes = nullptr;
					byteBuffer->Buffer(&bytes);

					unsigned int toWrite = buffer->Length;
					ssize_t numWritten = 0;
					unsigned int total = 0;

					do
					{
						numWritten = _write(_ctx, reinterpret_cast<const char*>(bytes), toWrite);
						if (numWritten < 0)
							break;

						total += numWritten;
						toWrite-= numWritten;
						bytes += numWritten;

						reporter.report(total);

					} while (numWritten > 0 && toWrite > 0);

					return total;
				});
			}
			virtual IAsyncOperation<bool>^ FlushAsync()
			{
				return create_async([]() { return true; });
			}
		};

		ref class File : public IStorageFile
		{
		private:
			ReadFn _readFn;
			WriteFn _writeFn;
			CloseFn _closeFn;
			void* _ctx;

		internal:
			File(ReadFn rfn, WriteFn wfn, CloseFn cfn, void* ctx) : _readFn(rfn), _writeFn(wfn), _closeFn(cfn), _ctx(ctx) {}

		public:
			virtual ~File() {}

		public:
			////////////////////////////////////////////////////////////////
			// Unsupported but required by the interface

			// IStorageFile

			virtual IAsyncAction^ CopyAndReplaceAsync(IStorageFile^ fileToReplace) { _UNEXPECTED_(nullptr); }
			virtual IAsyncOperation<StorageFile^>^ CopyAsync(IStorageFolder^ destinationFolder) { _UNEXPECTED_(nullptr); }
			virtual IAsyncOperation<StorageFile^>^ CopyAsync(IStorageFolder^ destinationFolder, String^ desiredNewName) { _UNEXPECTED_(nullptr); }
			virtual IAsyncOperation<StorageFile^>^ CopyAsync(IStorageFolder^ destinationFolder, String^ desiredNewName, NameCollisionOption option) { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ MoveAndReplaceAsync(IStorageFile^ fileToReplace) { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ MoveAsync(IStorageFolder^ destinationFolder) { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ MoveAsync(IStorageFolder^ destinationFolder, String^ desiredNewName) { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ MoveAsync(IStorageFolder^ destinationFolder, String^ desiredNewName, NameCollisionOption option) { _UNEXPECTED_(nullptr); }

			// IStorageItem

			virtual IAsyncAction^ DeleteAsync() { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ DeleteAsync(StorageDeleteOption option) { _UNEXPECTED_(nullptr); }
			virtual IAsyncOperation<FileProperties::BasicProperties^>^ GetBasicPropertiesAsync() { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ RenameAsync(String^ newName) { _UNEXPECTED_(nullptr); }
			virtual IAsyncAction^ RenameAsync(String^ newName, NameCollisionOption option) { _UNEXPECTED_(nullptr); }

			////////////////////////////////////////////////////////////////
			// Supported

			// IStorageFile

			virtual IAsyncOperation<Streams::IRandomAccessStream^>^ OpenAsync(FileAccessMode accessMode) {
				return create_async([this]() -> IRandomAccessStream^ {
					return ref new RandomWrapper(_readFn, _writeFn, _ctx);
				});
			}
			virtual IAsyncOperation<StorageStreamTransaction^>^ OpenTransactedWriteAsync() {
				return create_async([]() -> StorageStreamTransaction^ {
					return nullptr;
				});
			}
			property String^ ContentType
			{
				virtual String^ get() { return TEXT("application/xml"); }
			}
			property String^ FileType
			{
				virtual String^ get() { return TEXT("xml"); }
			}

			// IInputStreamReference

			virtual IAsyncOperation<IInputStream^>^ OpenSequentialReadAsync() {
				if (_ctx == nullptr)
					return nullptr;
				return create_async([this]() -> IInputStream^ {
					return ref new InputWrapper(_readFn, _ctx);
				});
			}

			// IRandomAccessStreamReference

			virtual IAsyncOperation<IRandomAccessStreamWithContentType^>^ OpenReadAsync() {
				return create_async([this]() -> IRandomAccessStreamWithContentType^ {
					return ref new RandomWrapper(_readFn, _writeFn, _ctx);
				});
			}

			// IStorageItem

			virtual bool IsOfType(StorageItemTypes type) { return type == StorageItemTypes::File; }
			property FileAttributes Attributes
			{
				virtual FileAttributes get() { return FileAttributes::Normal; }
			}
			property DateTime DateCreated
			{
				virtual DateTime get() { return DateTime(); }
			}
			property String^ Name
			{
				virtual String^ get() { return TEXT("unknown.xml"); }
			}
			property String^ Path
			{
				virtual String^ get() { return Name; }
			}

		};
	}
}

EPUB3_XML_BEGIN_NAMESPACE

InputBuffer::InputBuffer()
{
	_store = ref new ::Readium::XML::File(InputBuffer::read_cb, nullptr, InputBuffer::close_cb, (void*)this);
}
InputBuffer::InputBuffer(InputBuffer&& o) : _store(o._store)
{
	o._store = nullptr;
}
InputBuffer::~InputBuffer()
{
	this->close();
	_store = nullptr;
}
int InputBuffer::read_cb(void *context, char *buffer, int len)
{
	InputBuffer * p = reinterpret_cast<InputBuffer*>(context);
	return static_cast<int>(p->read(reinterpret_cast<uint8_t*>(buffer), static_cast<size_t>(len)));
}
int InputBuffer::close_cb(void *context)
{
	InputBuffer * p = static_cast<InputBuffer*>(context);
	return (p->close() ? 0 : -1);
}
std::shared_ptr<Document> InputBuffer::ReadDocument(const char* url, const char* encoding, XmlOptions options)
{
#if 0
	std::wstring wstr(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(url));
	String^ uriStr = ref new String(wstr.data(), wstr.length());
	Uri^ uri = ref new Uri(uriStr);
	auto task = XmlDocument::LoadFromUriAsync(uri);
	return new Document(task->GetResults());
#else
	using Converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>;
	static const size_t BUF_SIZE = 4096;
	std::wstring str;
	uint8_t buf[BUF_SIZE];
	Converter converter;
	int numRead = 0;

	do
	{
		numRead = static_cast<int>(this->read(buf, BUF_SIZE));
		if (numRead > 0)
		{
			str.append(converter.from_bytes(reinterpret_cast<char*>(buf), reinterpret_cast<char*>(buf + numRead)));
		}

	} while (numRead > 0);

	this->close();

	::Platform::String^ nstr = ref new String(str.data(), static_cast<unsigned int>(str.length()));
	str.clear();		// watch your memory
	XmlDocument^ native = ref new XmlDocument;
	XmlLoadSettings^ settings = ref new XmlLoadSettings;
	settings->ElementContentWhiteSpace = false;
	settings->MaxElementDepth = 100;
	settings->ProhibitDtd = ((options & PROHIBIT_DTD) == PROHIBIT_DTD);
	settings->ResolveExternals = ((options & RESOLVE_EXTERNALS) == RESOLVE_EXTERNALS);
	settings->ValidateOnParse = ((options & VALIDATE_ON_PARSE) == VALIDATE_ON_PARSE);
	
	try
	{
		native->LoadXml(nstr, settings);
		return std::make_shared<Document>(native);
	}
	catch (::Platform::Exception^ exc)
	{
		std::cerr << "WinRT Exception encountered: HRESULT=" << exc->HResult << ", Message=\"" << exc->Message->Data() << "\"" << std::endl;
	}
	catch (std::exception& exc)
	{
		std::cerr << "STL Exception encountered: what=\"" << exc.what() << "\"" << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception encountered." << std::endl;
	}
	
	return nullptr;
#endif
}

OutputBuffer::OutputBuffer(const std::string & encoding)
{
	_store = ref new ::Readium::XML::File(nullptr, OutputBuffer::write_cb, OutputBuffer::close_cb, (void*)this);
}
OutputBuffer::~OutputBuffer()
{
	this->close();
	_store = nullptr;
}
int OutputBuffer::write_cb(void *context, const char *buffer, int len)
{
	OutputBuffer * p = reinterpret_cast<OutputBuffer*>(context);
	return (p->write(reinterpret_cast<const uint8_t*>(buffer), static_cast<size_t>(len)) ? len : -1);
}
int OutputBuffer::close_cb(void *context)
{
	OutputBuffer * p = reinterpret_cast<OutputBuffer*>(context);
	return (p->close() ? 0 : -1);
}
int OutputBuffer::WriteDocument(std::shared_ptr<const Document> doc)
{
#if 0
	auto task = doc->xml()->SaveToFileAsync(_store);
	return 1;	// meh
#else
	String^ xmlstr = doc->xml()->GetXml();
	const wchar_t* wbuf = xmlstr->Data();
	size_t len = xmlstr->Length();

	using Converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>;
	std::string utf8(Converter().to_bytes(wbuf, wbuf + len));
	delete xmlstr;		// watch the memory usage

	const char* buf = utf8.data();
	size_t toWrite = utf8.length();

	do
	{
		int numWritten = this->write(reinterpret_cast<const uint8_t*>(buf), toWrite);
		if (numWritten < 0)
			break;
		buf += numWritten;
		toWrite -= numWritten;

	} while (toWrite > 0);

	this->close();
	return toWrite == 0;
#endif
}

size_t StreamInputBuffer::read(uint8_t *buf, size_t len)
{
	size_t num = 0;
	if (_input.good())
		num = static_cast<size_t>(_input.readsome(reinterpret_cast<std::istream::char_type*>(buf), len));
	return num;
}
bool StreamInputBuffer::close()
{
	return true;
}

bool StreamOutputBuffer::write(const uint8_t *buffer, size_t len)
{
	if (bool(_output))
		_output.write(reinterpret_cast<const std::ostream::char_type*>(buffer), len);
	return _output.good();
}
bool StreamOutputBuffer::close()
{
	_output.flush();
	return true;
}

EPUB3_XML_END_NAMESPACE
