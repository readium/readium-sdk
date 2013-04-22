//
//  ref_counted.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
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

#ifndef __ePub3__ref_counted__
#define __ePub3__ref_counted__

#include "basic.h"

struct adopt_ref_t {};

#if BUILDING_EPUB3
extern const adopt_ref_t adopt_ref;
#else
constexpr adopt_ref_t adopt_ref = adopt_ref_t();
#endif

EPUB3_BEGIN_NAMESPACE

///
/// A very simple reference-counting class. Does not self-delete.
class RefCountable
{
private:
    std::atomic_int     _refs;          ///< The number of references.
    
public:
    ///
    /// Initializes the reference count to 1.
    RefCountable() : _refs(1) {}
    virtual ~RefCountable() {}
    
    virtual void retain() { ++_refs; }
    virtual int release() { return --_refs; }
    
    void operator delete(void* o) {
        RefCountable* __p = reinterpret_cast<RefCountable*>(o);
        if ( __p->release() == 0 )
            ::delete __p;
    }
    
};

///
/// Wrapper for a pointer to a refcounted type. This deletes its content.
template <class _Tp>
class RefCounted
{
private:
    _Tp*    _ref;
    
public:
    RefCounted(std::nullptr_t = nullptr) : _ref(nullptr) {}
    RefCounted(_Tp* __p) : _ref(__p) { _ref->retain(); }
    RefCounted(_Tp* __p, adopt_ref_t) : _ref(__p) {}
    RefCounted(const RefCounted& o) : _ref(o._ref) { _ref->retain(); }
    RefCounted(RefCounted& o) : _ref(o._ref) { o._ref = nullptr; }
    ~RefCounted() { if (_ref != nullptr) delete _ref; }
    
    void swap(RefCounted& o) {
        std::swap(_ref, o._ref);
    }
    
    RefCounted& operator=(const RefCounted& o) {
        if (_ref != nullptr) {
            delete _ref;
        }
        o._ref->retain();
        _ref = o._ref;
        return *this;
    }
    RefCounted& operator=(RefCounted&& o) {
        if (_ref != nullptr) {
            delete _ref;
        }
        _ref = o._ref;
        o._ref = nullptr;
        return *this;
    }
    
    operator bool() const { return _ref != nullptr; }
    operator _Tp*() const { return _ref; }
    template <class _OTp>
    operator _OTp*() const { return dynamic_cast<_OTp*>(_ref); }
    
    bool operator==(const RefCounted& o) const { return _ref == o._ref; }
    bool operator==(_Tp* o) const { return _ref == o; }
    bool operator!=(const RefCounted& o) const { return _ref != o._ref; }
    bool operator!=(_Tp* o) const { return _ref != o; }
    
    _Tp& operator*() { return *_ref; }
    const _Tp& operator*() const { return *_ref; }
    _Tp* operator->() { return _ref; }
    const _Tp* operator->() const { return _ref; }
    
    _Tp Abandon() {
        _Tp* __r = _ref;
        _ref = nullptr;
        return __r;
    }
    
    const std::type_info& TypeInfo() const { return typeid(_Tp); }
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__ref_counted__) */
