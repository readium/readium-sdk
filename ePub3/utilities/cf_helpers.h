//
//  cf_helpers.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-09.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ePub3_cf_helpers_h
#define ePub3_cf_helpers_h

#include <ePub3/base.h>

EPUB3_BEGIN_NAMESPACE

#if EPUB_USE(CF)
class EPUB3_EXPORT cf_clock
{
public:
    typedef std::chrono::duration<CFTimeInterval>   duration;
    typedef duration::rep                           rep;
    typedef duration::period                        period;
    typedef std::chrono::time_point<cf_clock>       time_point;
    static const bool is_steady =                   false;
    
    static time_point   now()                               noexcept;
    static time_t       to_time_t(const time_point& __t)    noexcept;
    static time_point   from_time_t(const time_t& __t)      noexcept;
};

struct cf_adopt_ref_t {};

#if BUILDING_EPUB3
extern const cf_adopt_ref_t cf_adopt_ref;
#else
CONSTEXPR cf_adopt_ref_t cf_adopt_ref = cf_adopt_ref_t();
#endif

template <class _CF>
static inline _LIBCPP_INLINE_VISIBILITY
_CF __SafeCFRetain(_CF __cf)
{
    if (__cf == nullptr) return nullptr;
    return reinterpret_cast<_CF>(const_cast<void*>(CFRetain(__cf)));
}

template <class _CF>
static inline _LIBCPP_INLINE_VISIBILITY
void __SafeCFRelease(_CF __cf)
{
    if (__cf != nullptr) CFRelease(__cf);
}

template <class _CF1, class _CF2>
static inline _LIBCPP_INLINE_VISIBILITY
bool __SafeCFEqual(_CF1 __cf1, _CF2 __cf2)
{
    if (__cf1 == __cf2) return false;       // i.e. incomparable
    if (__cf1 == nullptr) return false;
    if (__cf2 == nullptr) return false;
    return static_cast<bool>(CFEqual(__cf1, __cf2));
}

template <class _CF>
class CFRefCounted
{
private:
    _CF         _ref;
    
public:
    CFRefCounted() : _ref(nullptr) {}
    CFRefCounted(std::nullptr_t) : _ref(nullptr) {}
    CFRefCounted(_CF __cf) : _ref(__SafeCFRetain(__cf)) {}
    CFRefCounted(_CF __cf, cf_adopt_ref_t) : _ref(__cf) {}
    CFRefCounted(const CFRefCounted<_CF>& o) : _ref(__SafeCFRetain(o._ref)) {}
    CFRefCounted(CFRefCounted<_CF>&& o) : _ref(o._ref) {
        o._ref = nullptr;
    }
    ~CFRefCounted() {
        __SafeCFRelease(_ref);
    }
    
    void swap(CFRefCounted& o) {
        std::swap(_ref, o._ref);
    }
    
    bool operator==(const CFRefCounted& o) const {
        return __SafeCFEqual(_ref, o._ref);
    }
    bool operator==(_CF __cf) const {
        return __SafeCFEqual(_ref, __cf);
    }
    bool operator!=(const CFRefCounted& o) const {
        return !(this->operator==(o));
    }
    bool operator!=(_CF __cf) const {
        return !(this->operator==(__cf));
    }
    
    CFRefCounted& operator=(const CFRefCounted& o) {
        __SafeCFRelease(_ref);
        _ref = __SafeCFRetain(o._ref);
        return *this;
    }
    CFRefCounted& operator=(CFRefCounted&& o) {
        __SafeCFRelease(_ref);
        _ref = o._ref;
        o._ref = nullptr;
        return *this;
    }
    CFRefCounted& operator=(_CF __cf) {
        __SafeCFRelease(_ref);
        _ref = __SafeCFRetain(__cf);
        return *this;
    }
    
    operator bool() const   { return _ref != nullptr; }
    operator _CF()  const   { return _ref; }
    
    _CF Abandon() {
        _CF __r = _ref;
        _ref = nullptr;
        return __r;
    }
    
};
#endif // EPUB_USE(CF)

EPUB3_END_NAMESPACE

#endif
