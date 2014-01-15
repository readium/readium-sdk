#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <wrl/implements.h>
#include <robuffer.h>
#include <windows.storage.streams.h>

using ::Windows::Storage::Streams::IBuffer;
using ::Windows::Storage::Streams::IBufferByteAccess;

namespace PhoneSupportInterfaces
{

	class ByteBuffer : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
							  ABI::Windows::Storage::Streams::IBuffer, Windows::Storage::Streams::IBufferByteAccess>
	{
		InspectableClass(TEXT("RuntimeClass_PhoneSupport_ByteBuffer"), BaseTrust);

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
				Microsoft::WRL::ComPtr<ByteBuffer> result;
				Microsoft::WRL::Details::MakeAndInitialize<ByteBuffer>(&result, totalSize);
				auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
				return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
			}

		static inline
			::Windows::Storage::Streams::IBuffer^
			MakeBuffer(byte* bytes, UINT totalSize)
		{
				Microsoft::WRL::ComPtr<ByteBuffer> result;
				Microsoft::WRL::Details::MakeAndInitialize<ByteBuffer>(&result, bytes, totalSize);
				auto inspectable = reinterpret_cast<IInspectable*>(result.Get());
				return reinterpret_cast<::Windows::Storage::Streams::IBuffer^>(inspectable);
			}

		virtual ~ByteBuffer()
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

	public ref class BufferFactory sealed
	{
	public:
		BufferFactory() {}
		virtual ~BufferFactory() {}

		static ::Windows::Storage::Streams::IBuffer^ Create(UINT capacity)
		{
			return ByteBuffer::MakeBuffer(capacity);
		}

		static ::Windows::Storage::Streams::IBuffer^ CreateFromBytes(byte* bytes, UINT len)
		{
			return ByteBuffer::MakeBuffer(bytes, len);
		}
	};

}