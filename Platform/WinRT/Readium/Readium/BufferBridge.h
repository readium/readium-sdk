//
//  BufferBridge.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-03.
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

#ifndef __Readium_Buffer_Bridge_h__
#define __Readium_Buffer_Bridge_h__

#include "Readium.h"

#include <wrl/client.h>
#include <robuffer.h>
#include <windows.storage.streams.h>
#include <ePub3/utilities/byte_buffer.h>

BEGIN_READIUM_API

class BridgedBuffer : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
														  ABI::Windows::Storage::Streams::IBuffer, Windows::Storage::Streams::IBufferByteAccess>
{
	InspectableClass(TEXT("RuntimeClass_Readium_BridgedBuffer"), BaseTrust);

private:
	byte*	_bytes;
	size_t	_length;
	size_t	_capacity;
	bool	_allocated;


public:
	static inline
	::Windows::Storage::Streams::IBuffer^
	MakeBuffer(UINT totalSize)
	{
		Microsoft::WRL::ComPtr<BridgedBuffer> result;
		Microsoft::WRL::Details::MakeAndInitialize<BridgedBuffer>(&result, totalSize);
		auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
		return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
	}

	static inline
	::Windows::Storage::Streams::IBuffer^
	MakeBuffer(byte* bytes, UINT totalSize)
	{
		Microsoft::WRL::ComPtr<BridgedBuffer> result;
		Microsoft::WRL::Details::MakeAndInitialize<BridgedBuffer>(&result, bytes, totalSize);
		auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
		return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
	}

	virtual ~BridgedBuffer()
	{
		if (_allocated)
			delete[] _bytes;
	}

	STDMETHODIMP RuntimeClassInitialize(UINT totalSize)
	{
		_capacity = totalSize;
		_length = 0;
		_bytes = new byte[totalSize];
		_allocated = true;
		return S_OK;
	}

	STDMETHODIMP RuntimeClassInitialize(byte* bytes, UINT len)
	{
		_length = _capacity = len;
		_bytes = bytes;
		_allocated = false;
		return S_OK;
	}

	STDMETHODIMP Buffer(byte **value)
	{
		*value = _bytes;
		return S_OK;
	}

	STDMETHODIMP get_Capacity(UINT32 *value)
	{
		*value = _capacity;
		return S_OK;
	}

	STDMETHODIMP get_Length(UINT32 *value)
	{
		*value = _length;
		return S_OK;
	}

	STDMETHODIMP put_Length(UINT32 value)
	{
		if (value > _length)
			return E_INVALIDARG;
		_length = value;
		return S_OK;
	}

};

class BridgedByteBuffer : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
															  ABI::Windows::Storage::Streams::IBuffer, Windows::Storage::Streams::IBufferByteAccess>
{
private:
	::ePub3::ByteBuffer*	_buf;
	bool					_ownsBuf;

public:
	static inline
	::Windows::Storage::Streams::IBuffer^
	MakeBuffer(UINT totalSize)
	{
		Microsoft::WRL::ComPtr<BridgedByteBuffer> result;
		Microsoft::WRL::Details::MakeAndInitialize<BridgedByteBuffer>(&result, totalSize);
		auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
		return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
	}

	static inline
	::Windows::Storage::Streams::IBuffer^
	MakeBuffer(::ePub3::ByteBuffer& buf)
	{
		Microsoft::WRL::ComPtr<BridgedByteBuffer> result;
		Microsoft::WRL::Details::MakeAndInitialize<BridgedByteBuffer>(&result, buf);
		auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
		return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
	}

	static inline
	::Windows::Storage::Streams::IBuffer^
	MakeBuffer(::ePub3::ByteBuffer&& buf)
	{
		Microsoft::WRL::ComPtr<BridgedByteBuffer> result;
		Microsoft::WRL::Details::MakeAndInitialize<BridgedByteBuffer>(&result, std::move(buf));
		auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
		return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
	}

	virtual ~BridgedByteBuffer()
	{
		if (_ownsBuf)
			delete _buf;
	}

	STDMETHODIMP RuntimeClassInitialize(UINT totalSize)
	{
		_buf = new ::ePub3::ByteBuffer(totalSize, ::ePub3::prealloc_buf);
		_ownsBuf = true;
		return S_OK;
	}

	STDMETHODIMP RuntimeClassInitialize(::ePub3::ByteBuffer& inBuf)
	{
		_buf = &inBuf;
		_ownsBuf = false;
		return S_OK;
	}

	STDMETHODIMP RuntimeClassInitialize(::ePub3::ByteBuffer&& inBuf)
	{
		_buf = new ::ePub3::ByteBuffer(std::move(inBuf));
		_ownsBuf = true;
		return S_OK;
	}

	STDMETHODIMP Buffer(byte **value)
	{
		*value = _buf->GetBytes();
		return S_OK;
	}

	STDMETHODIMP get_Capacity(UINT32 *value)
	{
		*value = _buf->m_bufferCapacity;
		return S_OK;
	}

	STDMETHODIMP get_Length(UINT32 *value)
	{
		*value = _buf->GetBufferSize();
		return S_OK;
	}

	STDMETHODIMP put_Length(UINT32 value)
	{
		if (value > _buf->m_bufferSize)
			return E_INVALIDARG;
		_buf->m_bufferSize = value;
		return S_OK;
	}
};

byte* GetIBufferBytes(::Windows::Storage::Streams::IBuffer^ buffer)
{
	Microsoft::WRL::ComPtr<IInspectable> inspectable(reinterpret_cast<IInspectable*>(buffer));
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bytes = nullptr;
	__abi_ThrowIfFailed(inspectable.As(&bytes));

	byte* result = nullptr;
	__abi_ThrowIfFailed(bytes->Buffer(&result));

	return result;
}

END_READIUM_API

#endif	/* __Readium_Buffer_Bridge_h__ */
