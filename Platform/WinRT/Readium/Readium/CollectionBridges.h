//
//  CollectionBridges.h
//  Readium
//
//  Created by Jim Dovey on 2013-09-27.
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

#ifndef __Readium_CollectionBridges_h__
#define __Readium_CollectionBridges_h__

#include "Readium.h"
#include <collection.h>
#include <vector>

BEGIN_READIUM_API

#define _COLLECTION_ATTRIBUTES [::Platform::Metadata::RuntimeClassName] [::Windows::Foundation::Metadata::Default]

#if EPUB_PLATFORM(WIN_PHONE)
# define _COLLECTION_WUXI 0
#else
#define _COLLECTION_WUXI 1
#endif

#define _COLLECTION_TRANSLATE           \
} catch (const ::std::bad_alloc&) { \
	throw ref new OutOfMemoryException; \
} catch (const ::std::exception&) { \
	throw ref new FailureException;     \
}

namespace Details = ::Platform::Collections::Details;
namespace WUXI = ::Windows::UI::Xaml::Interop;
namespace WFC = ::Windows::Foundation::Collections;

using ::Platform::String;
using ::Platform::Object;
using ::Platform::WriteOnlyArray;
using ::Platform::Collections::Vector;
using ::Platform::Collections::VectorView;
using ::Platform::Collections::MapView;
using Details::Wrap;
using Details::Unwrap;

template <typename K, typename V, typename NK, typename NV, typename KIn, typename KOut, typename VIn, typename VOut, typename C = ::std::less<NK>>
ref class BridgedMap;

template <class T>
struct __remove_hat {
	typedef typename T type;
};
template <class T>
struct __remove_hat<T^> {
	typedef typename T type;
};

template <typename Bridge, typename Native>
struct ToBridged : public ::std::unary_function<Native, Bridge>
{
	Bridge operator()(Native n) {
		return __remove_hat<Bridge>::type::Wrapper(n);
	}
};
template <typename Bridge, typename Native>
struct ToNative : public ::std::unary_function<Bridge, Native>
{
	Native operator()(Bridge b) {
		return b->NativeObject;
	}
};

struct ToBridgedString : public ::std::unary_function<const ::ePub3::string&, ::Platform::String^>
{
	::Platform::String^ operator()(const ::ePub3::string& n) {
		return StringFromNative(n);
	}
};

struct ToNativeString : public ::std::unary_function<::Platform::String^, ::ePub3::string>
{
	::ePub3::string operator()(::Platform::String^ b) {
		return StringToNative(b);
	}
};

template <typename K, typename V, typename NK, typename NV, typename KIn, typename KOut, typename VIn, typename VOut>
ref class BridgedKeyValuePair sealed : public _COLLECTION_ATTRIBUTES WFC::IKeyValuePair<K, V>{
internal:
	BridgedKeyValuePair(const typename Details::Wrap<NK>::type& key, const typename Details::Wrap<NV>::type& value)
		: m_key(key), m_value(value) { }

public:
	virtual property K Key {
		virtual K get() {
			return KOut()(Unwrap(m_key));
		}
	}

	virtual property V Value {
		virtual V get() {
			return VOut()(Unwrap(m_value));
		}
	}

private:
	typename Wrap<NK>::type m_key;
	typename Wrap<NV>::type m_value;
};

template <typename K, typename V, typename NK, typename NV, typename KIn, typename KOut, typename VIn, typename VOut, typename WrappedMap>
ref class BridgedMapIterator : public _COLLECTION_ATTRIBUTES Details::WFC::IIterator<Details::WFC::IKeyValuePair<K, V>^>
{
internal:
	BridgedMapIterator(const ::std::shared_ptr<unsigned int>& ctr, const ::std::shared_ptr<WrappedMap>& m)
		: m_ctr(ctr), m_map(m), m_good_ctr(*ctr), m_iter(m->begin()) { }

public:
	virtual property WFC::IKeyValuePair<K, V>^ Current {
		virtual WFC::IKeyValuePair<K, V>^ get() {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			Details::ValidateBounds(m_iter != m_map->end());

			return ref new BridgedKeyValuePair<K, V, NK, NV, KIn, KOut, VIn, VOut>(m_iter->first, m_iter->second);
		}
	}

	virtual property bool HasCurrent {
		virtual bool get() {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			return m_iter != m_map->end();
		}
	}

	virtual bool MoveNext() {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		Details::ValidateBounds(m_iter != m_map->end());

		++m_iter;
		return m_iter != m_map->end();
	}

	virtual unsigned int GetMany(WriteOnlyArray<WFC::IKeyValuePair<K, V>^>^ dest) {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		unsigned int capacity = dest->Length;

		unsigned int actual = 0;

		while (capacity > 0 && m_iter != m_map->end()) {
			dest->set(actual, ref new BridgedKeyValuePair<K, V, NK, NV, KIn, KOut, VIn, VOut>(m_iter->first, m_iter->second));
			++m_iter;
			--capacity;
			++actual;
		}

		return actual;
	}

private:
	::std::shared_ptr<unsigned int> m_ctr;
	::std::shared_ptr<WrappedMap> m_map;
	unsigned int m_good_ctr;
	typename WrappedMap::const_iterator m_iter;
};

template <
	typename K, typename V,
	typename NK, typename NV,
	typename KIn = ToNativeString, typename KOut = ToBridgedString,
	typename VIn = ToNative<V, NV>, typename VOut = ToBridged<V, NV>,
	typename C = ::std::less<NK>
>
ref class BridgedMapView sealed : public _COLLECTION_ATTRIBUTES Details::WFC::IMapView<K, V>{
private:
	typedef typename Details::WrapMap<NK, NV, C>::type									WrappedMap;
	typedef		   BridgedMapIterator<K, V, NK, NV, KIn, KOut, VIn, VOut, WrappedMap>	MyIterator;

internal:
	explicit BridgedMapView(const C& comp = C()) {
		Details::Init(m_ctr, m_map, comp);

		m_good_ctr = 0;
	}

	explicit BridgedMapView(const ::std::map<NK, NV, C>& m) {
		Details::Init(m_ctr, m_map, m.key_comp());

		Details::EmplaceWrappedRange<NK, NV>(*m_map, m.begin(), m.end());

		m_good_ctr = 0;
	}

	explicit BridgedMapView(::std::map<NK, NV, C>&& m) {
		Details::InitMoveMap<NK, NV>(m_ctr, m_map, ::std::move(m));

		m_good_ctr = 0;
	}

	template <typename InIt> BridgedMapView(InIt first, InIt last, const C& comp = C()) {
		Details::Init(m_ctr, m_map, comp);

		Details::EmplaceWrappedRange<NK, NV>(*m_map, first, last);

		m_good_ctr = 0;
	}
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	BridgedMapView(::std::initializer_list < ::std::pair < const NK, NV >> il, const C& comp = C()) {
		Details::Init(m_ctr, m_map, comp);

		Details::EmplaceWrappedRange<NK, NV>(*m_map, il.begin(), il.end());

		m_good_ctr = 0;
	}
#endif

public:
	virtual Details::WFC::IIterator<Details::WFC::IKeyValuePair<K, V>^>^ First() {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		return ref new MyIterator(m_ctr, m_map);
	}

	virtual V Lookup(K key) {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		auto i = m_map->find(Details::MakeWrap(KIn()(key)));

		Details::ValidateBounds(i != m_map->end());

		return Details::Unwrap(VOut()(i->second));
	}

	virtual property unsigned int Size {
		virtual unsigned int get() {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			return static_cast<unsigned int>(m_map->size());
		}
	}

	virtual bool HasKey(K key) {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		return m_map->find(Details::MakeWrap(KIn()(key))) != m_map->end();
	}

	virtual void Split(Details::WFC::IMapView<K, V>^ * firstPartition, Details::WFC::IMapView<K, V>^ * secondPartition) {
		*firstPartition = nullptr;
		*secondPartition = nullptr;

		Details::ValidateCounter(m_ctr, m_good_ctr);
	}

private:
	BridgedMapView(const ::std::shared_ptr<unsigned int>& ctr, const ::std::shared_ptr<WrappedMap>& m)
		: m_ctr(ctr), m_map(m), m_good_ctr(*ctr) { }

	::std::shared_ptr<unsigned int> m_ctr;
	::std::shared_ptr<WrappedMap> m_map;
	unsigned int m_good_ctr;
};

template <typename T, typename NT, typename TOut>
inline unsigned int BridgedVectorGetMany(const ::std::vector<typename Wrap<NT>::type>& v, unsigned int startIndex, WriteOnlyArray<T>^ dest) {
	unsigned int capacity = dest->Length;

	unsigned int actual = static_cast<unsigned int>(v.size()) - startIndex;

	if (actual > capacity) {
		actual = capacity;
	}

	for (unsigned int i = 0; i < actual; ++i) {
		dest->set(i, Unwrap(TOut()(v[startIndex + i])));
	}

	return actual;
}

template <typename T, typename NT, typename TIn, typename TOut>
ref class IteratorForBridgedVectorView sealed
: public _COLLECTION_ATTRIBUTES WFC::IIterator<T>
#if _COLLECTION_WUXI
, public WUXI::IBindableIterator
#endif // _COLLECTION_WUXI
{
private:
	typedef ::std::vector<typename Wrap<NT>::type>	WrappedVector;
	typedef WFC::IIterator<T>						WFC_Base;

#if _COLLECTION_WUXI
	typedef WUXI::IBindableIterator WUXI_Base;
#endif // _COLLECTION_WUXI

internal:
	IteratorForBridgedVectorView(const ::std::shared_ptr<unsigned int>& ctr, const ::std::shared_ptr<WrappedVector>& vec)
		: m_ctr(ctr), m_vec(vec), m_good_ctr(*ctr), m_index(0) { }

public:
	virtual property T Current {
		virtual T get() = WFC_Base::Current::get {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			Details::ValidateBounds(m_index < m_vec->size());

			return Unwrap(TOut()((*m_vec)[m_index]));
		}
	}

	virtual property bool HasCurrent {
		virtual bool get() {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			return m_index < m_vec->size();
		}
	}

	virtual bool MoveNext() {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		Details::ValidateBounds(m_index < m_vec->size());

		++m_index;
		return m_index < m_vec->size();
	}

	virtual unsigned int GetMany(WriteOnlyArray<T>^ dest) {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		unsigned int actual = BridgedVectorGetMany<T, NT, TOut>(*m_vec, m_index, dest);

		m_index += actual;

		return actual;
	}

private:

#if _COLLECTION_WUXI
	virtual Object^ BindableCurrent() = WUXI_Base::Current::get {
		return Current;
	}
#endif // _COLLECTION_WUXI

	::std::shared_ptr<unsigned int> m_ctr;
	::std::shared_ptr<WrappedVector> m_vec;
	unsigned int m_good_ctr;
	unsigned int m_index;
};

#if _COLLECTION_WUXI
template <typename NT, typename E>
inline bool BridgedVectorBindableIndexOf(const ::std::vector<typename Wrap<NT>::type>& v, NT o, unsigned int * index) {
	unsigned int idx = 0;
	for (auto& r : v) {
		if (E()(o, r)) {
			*index = idx;
			return true;
		}
		++idx;
	}
	*index = v.size();
	return false;
}
#endif	// _COLLECTION_WUXI

template <
	typename T, typename NT,
	typename TIn, typename TOut, 
	typename E=::std::equal_to<NT>
>
ref class BridgedVectorView sealed
: public _COLLECTION_ATTRIBUTES Details::WFC::IVectorView<T>
#if _COLLECTION_WUXI
, public Details::WUXI::IBindableVectorView
#endif // _COLLECTION_WUXI
{
private:
	typedef ::std::vector<typename Details::Wrap<NT>::type>	WrappedVector;
	typedef Details::WFC::IVectorView<T>					WFC_Base;

#if _COLLECTION_WUXI
	typedef Details::WUXI::IBindableVectorView				WUXI_Base;
#endif // _COLLECTION_WUXI

internal:
	BridgedVectorView() {
		Details::Init(m_ctr, m_vec);

		m_good_ctr = 0;
	}

	explicit BridgedVectorView(unsigned int size) {
		Details::Init(m_ctr, m_vec, size);

		m_good_ctr = 0;
	}

	BridgedVectorView(unsigned int size, T value) {
		Details::Init(m_ctr, m_vec, size, Details::MakeWrap(TIn()(value)));

		m_good_ctr = 0;
	}
#if EPUB_PLATFORM(WIN_PHONE)
	explicit BridgedVectorView(const ::std::vector<NT>& v) {
		Details::Init(m_ctr, m_vec, v.begin(), v.end());

		m_good_ctr = 0;
	}
	explicit BridgedVectorView(::std::vector<NT>&& v) {
		Details::InitMoveVector(m_ctr, m_vec, ::std::move(v));

		m_good_ctr = 0;
	}
#else
	template <typename U> explicit BridgedVectorView(const ::std::vector<U>& v, typename Details::VectorEnableIf<NT, U>::type unused = nullptr) {
		Details::Init(m_ctr, m_vec, v.begin(), v.end());

		m_good_ctr = 0;
	}

	template <typename U> explicit BridgedVectorView(::std::vector<U>&& v, typename Details::VectorEnableIf<NT, U>::type unused = nullptr) {
		Details::InitMoveVector(m_ctr, m_vec, ::std::move(v));

		m_good_ctr = 0;
	}
#endif

	BridgedVectorView(const NT * ptr, unsigned int size) {
		Details::Init(m_ctr, m_vec, ptr, ptr + size);

		m_good_ctr = 0;
	}

	template <size_t N> explicit BridgedVectorView(const NT(&arr)[N]) {
		Details::Init(m_ctr, m_vec, arr, arr + N);

		m_good_ctr = 0;
	}

	template <size_t N> explicit BridgedVectorView(const ::std::array<NT, N>& a) {
		Details::Init(m_ctr, m_vec, a.begin(), a.end());

		m_good_ctr = 0;
	}

	template <typename InIt> BridgedVectorView(InIt first, InIt last) {
		// SFINAE is unnecessary here.

		Details::Init(m_ctr, m_vec, first, last);

		m_good_ctr = 0;
	}
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
	BridgedVectorView(::std::initializer_list<NT> il) {
		Details::Init(m_ctr, m_vec, il.begin(), il.end());

		m_good_ctr = 0;
	}
#endif

public:
	virtual Details::WFC::IIterator<T>^ First() = WFC_Base::First{
		Details::ValidateCounter(m_ctr, m_good_ctr);

		return ref new IteratorForBridgedVectorView<T, NT, TIn, TOut>(m_ctr, m_vec);
	  }

	virtual T GetAt(unsigned int index) = WFC_Base::GetAt{
		Details::ValidateCounter(m_ctr, m_good_ctr);

		Details::ValidateBounds(index < m_vec->size());

		return Details::Unwrap(TOut()((*m_vec)[index]));
	}

	virtual property unsigned int Size {
		virtual unsigned int get() {
			Details::ValidateCounter(m_ctr, m_good_ctr);

			return static_cast<unsigned int>(m_vec->size());
		}
	}

	virtual bool IndexOf(T value, unsigned int * index) = WFC_Base::IndexOf{
		*index = 0;

		Details::ValidateCounter(m_ctr, m_good_ctr);

		return Details::VectorIndexOf<NT, E>(*m_vec, TIn()(value), index);
	}

	virtual unsigned int GetMany(unsigned int startIndex, WriteOnlyArray<T>^ dest) {
		Details::ValidateCounter(m_ctr, m_good_ctr);

		Details::ValidateBounds(startIndex <= m_vec->size());

		return BridgedVectorGetMany<T, NT, TOut>(*m_vec, startIndex, dest);
	}

private:
	friend ref class Vector<NT, E>;

	BridgedVectorView(const ::std::shared_ptr<unsigned int>& ctr, const ::std::shared_ptr<WrappedVector>& vec)
		: m_ctr(ctr), m_vec(vec), m_good_ctr(*ctr) { }

#if _COLLECTION_WUXI
	virtual Details::WUXI::IBindableIterator^ BindableFirst() = WUXI_Base::First{
		return safe_cast<Details::WUXI::IBindableIterator^>(First());
	}

	virtual Object^ BindableGetAt(unsigned int index) = WUXI_Base::GetAt{
		return GetAt(index);
	}

	virtual bool BindableIndexOf(Object^ value, unsigned int * index) = WUXI_Base::IndexOf{
		*index = 0;

		Details::ValidateCounter(m_ctr, m_good_ctr);

		return BridgedVectorBindableIndexOf<NT, E>(*m_vec, TIn()((T)value), index);
	}
#endif // _COLLECTION_WUXI

	::std::shared_ptr<unsigned int> m_ctr;
	::std::shared_ptr<WrappedVector> m_vec;
	unsigned int m_good_ctr;
};

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
using BridgedStringVectorView = BridgedVectorView<::Platform::String^, ::ePub3::string, ToNativeString, ToBridgedString>;
using BridgedStringToStringMapView = BridgedMapView<::Platform::String^, ::Platform::String^, ::ePub3::string, ePub3::string, ToNativeString, ToBridgedString, ToNativeString, ToBridgedString>;

template <typename Bridge, typename Native>
using BridgedObjectVectorView = BridgedVectorView<Bridge, Native, ToNative<Bridge, Native>, ToBridged<Bridge, Native>>;
#define BRIDGED_OBJECT_VECTOR(Bridge, Native) BridgedObjectVectorView<Bridge, Native>

template <typename Bridge, typename Native>
using BridgedStringKeyedObjectMapView = BridgedMapView<::Platform::String^, Bridge, ::ePub3::string, Native>;
#define BRIDGED_STRING_OBJECT_MAP(Bridge, Native) BridgedStringKeyedObjectMapView<Bridge, Native>

template <typename BKey, typename NKey, typename BValue, typename NValue>
using BridgedObjectKeyedObjectMapView = BridgedMapView<BKey, BValue, NKey, NValue, ToNative<BKey, NKey>, ToBridged<BKey, NKey>>;
#define BRIDGED_OBJECT_MAP(BKey, NKey, BValue, NValue) BridgedObjectKeyedObjectMapView<BKey, NKey, BValue, NValue>
#else
typedef BridgedVectorView<::Platform::String^, ::ePub3::string, ToNativeString, ToBridgedString> BridgedStringVectorView;
typedef BridgedMapView<::Platform::String^, ::Platform::String^, ::ePub3::string, ePub3::string, ToNativeString, ToBridgedString, ToNativeString, ToBridgedString> BridgedStringToStringMapView;

#define BRIDGED_OBJECT_VECTOR(Bridge, Native) BridgedVectorView<Bridge, Native, ToNative<Bridge, Native>, ToBridged<Bridge, Native>>
#define BRIDGED_STRING_OBJECT_MAP(Bridge, Native) BridgedMapView<::Platform::String^, Bridge, ::ePub3::string, Native>
#define BRIDGED_OBJECT_MAP(BKey, NKey, BValue, NValue) BridgedMapView<BKey, BValue, NKey, NValue, ToNative<BKey, NKey>, ToBridged<BKey, NKey>>
#endif

END_READIUM_API

#undef _COLLECTION_ATTRIBUTES
#undef _COLLECTION_WUXI
#undef _COLLECTION_TRANSLATE

#endif	/* __Readium_CollectionBridges_h__ */
