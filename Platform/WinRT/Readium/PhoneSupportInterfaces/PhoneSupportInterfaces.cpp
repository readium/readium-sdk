// PhoneSupportInterfaces.cpp
#include "pch.h"
#include "PhoneSupportInterfaces.h"
#include <atomic>

using namespace PhoneSupportInterfaces;
using namespace Platform;

// should be static members of XmlFactoryGlue, but:
// "error: member of a WinRT class cannot be of a non-WinRT type."
// ...cunts.
static std::atomic<PhoneSupportInterfaces::FactoryGlue^>	_singleton(nullptr);
static std::mutex											_singleton_lock;

namespace PhoneSupportInterfaces {

	FactoryGlue::FactoryGlue()
		: _xmlFactory(nullptr), _cryptoFactory(nullptr)
	{
	}
	FactoryGlue::~FactoryGlue()
	{
	}
	IXmlDocumentFactory^ FactoryGlue::XmlFactory::get()
	{
		return _xmlFactory;
	}
	void FactoryGlue::XmlFactory::set(IXmlDocumentFactory^ obj)
	{
		_xmlFactory = obj;
	}
	ICryptoFactory^ FactoryGlue::CryptoFactory::get()
	{
		return _cryptoFactory;
	}
	void FactoryGlue::CryptoFactory::set(ICryptoFactory^ obj)
	{
		_cryptoFactory = obj;
	}

	FactoryGlue^ FactoryGlue::Singleton()
	{
		// courtesy of "Double-Checked Locking Is Fixed In C++11" by Jeff Preshing,
		// located at http://preshing.com/20130930/double-cucked-locking-is-fixed-in-cpp11/
		FactoryGlue^ tmp = _singleton.load(std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_acquire);
		if (tmp == nullptr)
		{
			std::lock_guard<std::mutex> lock(_singleton_lock);
			tmp = _singleton.load(std::memory_order_relaxed);
			if (tmp == nullptr)
			{
				tmp = ref new FactoryGlue;
				std::atomic_thread_fence(std::memory_order_release);
				_singleton.store(tmp, std::memory_order_relaxed);
			}
		}

		return tmp;
	}

}
