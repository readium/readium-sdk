//
//  optional.h
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
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

#ifndef ePub3_optional_h
#define ePub3_optional_h

#include <ePub3/epub3.h>

/*

        Header <experimental/optional> synopsis

namespace std {
namespace experimental {

  // X.Y.4, optional for object types
  template <class _Tp> class optional;

  // X.Y.5, In-place construction
  struct in_place_t{};
  constexpr in_place_t in_place{};

  // X.Y.6, Disengaged state indicator
  struct nullopt_t{see below};
  constexpr nullopt_t nullopt(unspecified);
  
  // X.Y.7, class bad_optional_access
  class bad_optional_access;

  // X.Y.8, Relational operators
  template <class _Tp>
    constexpr bool operator==(const optional<_Tp>&, const optional<_Tp>&);
  template <class _Tp>
    constexpr bool operator!=(const optional<_Tp>&, const optional<_Tp>&);
  template <class _Tp>
    constexpr bool operator<(const optional<_Tp>&, const optional<_Tp>&);
  template <class _Tp>
    constexpr bool operator>(const optional<_Tp>&, const optional<_Tp>&);
  template <class _Tp>
    constexpr bool operator<=(const optional<_Tp>&, const optional<_Tp>&);
  template <class _Tp>
    constexpr bool operator>=(const optional<_Tp>&, const optional<_Tp>&);

  // X.Y.9, Comparison with nullopt
  template <class _Tp> constexpr bool operator==(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator==(nullopt_t, const optional<_Tp>&) _NOEXCEPT;
  template <class _Tp> constexpr bool operator!=(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator!=(nullopt_t, const optional<_Tp>&) _NOEXCEPT;
  template <class _Tp> constexpr bool operator<(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator<(nullopt_t, const optional<_Tp>&) _NOEXCEPT;
  template <class _Tp> constexpr bool operator<=(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator<=(nullopt_t, const optional<_Tp>&) _NOEXCEPT;
  template <class _Tp> constexpr bool operator>(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator>(nullopt_t, const optional<_Tp>&) _NOEXCEPT;
  template <class _Tp> constexpr bool operator>=(const optional<_Tp>&, nullopt_t) _NOEXCEPT;
  template <class _Tp> constexpr bool operator>=(nullopt_t, const optional<_Tp>&) _NOEXCEPT;

  // X.Y.10, Comparison with T
  template <class _Tp> constexpr bool operator==(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator==(const T&, const optional<_Tp>&);
  template <class _Tp> constexpr bool operator!=(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator!=(const T&, const optional<_Tp>&);
  template <class _Tp> constexpr bool operator<(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator<(const T&, const optional<_Tp>&);
  template <class _Tp> constexpr bool operator<=(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator<=(const T&, const optional<_Tp>&);
  template <class _Tp> constexpr bool operator>(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator>(const T&, const optional<_Tp>&);
  template <class _Tp> constexpr bool operator>=(const optional<_Tp>&, const T&);
  template <class _Tp> constexpr bool operator>=(const T&, const optional<_Tp>&);

  // X.Y.11, Specialized algorithms
  template <class _Tp> void swap(optional<_Tp>&, optional<_Tp>&) _NOEXCEPT(see below);
  template <class _Tp> constexpr optional<see below> make_optional(T&&);

  // X.Y.12, hash support
  template <class _Tp> struct hash;
  template <class _Tp> struct hash<optional<_Tp>>;

  template <class _Tp>
  class optional
  {
  public:
    typedef T value_type;

    // X.Y.4.1, constructors
    constexpr optional() _NOEXCEPT;
    constexpr optional(nullopt_t) _NOEXCEPT;
    optional(const optional&);
    optional(optional&&) _NOEXCEPT(see below);
    constexpr optional(const T&);
    constexpr optional(T&&);
    template <class... Args> constexpr explicit optional(in_place_t, Args&&...);
    template <class U, class... Args>
      constexpr explicit optional(in_place_t, initializer_list<U>, Args&&...);

    // X.Y.4.2, destructor
    ~optional();

    // X.Y.4.3, assignment
    optional& operator=(nullopt_t) _NOEXCEPT;
    optional& operator=(const optional&);
    optional& operator=(optional&&) _NOEXCEPT(see below);
    template <class U> optional& operator=(U&&);
    template <class... Args> void emplace(Args&&...);
    template <class U, class... Args>
      void emplace(initializer_list<U>, Args&&...);

    // X.Y.4.4, swap
    void swap(optional&) _NOEXCEPT(see below);

    // X.Y.4.5, observers
    constexpr T const* operator ->() const;
    T* operator ->();
    constexpr T const& operator *() const;
    T& operator *();
    constexpr explicit operator bool() const _NOEXCEPT;
    constexpr T const& value() const;
    T& value();
    template <class U> constexpr T value_or(U&&) const&;
    template <class U> T value_or(U&&) &&;

  private:
    bool init; // exposition only
    T*   val;  // exposition only
  };

} // namespace experimental
} // namespace std

*/

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <cassert>
#include <functional>
#include <string>
#include <stdexcept>

#define HAS_CONSTEXPR_OPTIONAL (EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR) && EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES))

EPUB3_BEGIN_NAMESPACE

// X.Y.4, optional for object types
template <class _Tp> class optional;

// workaround: std utility functions aren't constexpr yet
template <class _Tp>
inline CONSTEXPR
_Tp&&
__constexpr_forward(typename std::remove_reference<_Tp>::type& __t) _NOEXCEPT
{
    return static_cast<_Tp&&>(__t);
}

template <class _Tp>
inline CONSTEXPR
_Tp&&
__constexpr_forward(typename std::remove_reference<_Tp>::type&& __t) _NOEXCEPT
{
    static_assert(!std::is_lvalue_reference<_Tp>::value, "Can not forward an rvalue as an lvalue");
    return static_cast<_Tp&&>(__t);
}

template <class _Tp>
inline CONSTEXPR
typename std::remove_reference<_Tp>::type&&
__constexpr_move(_Tp&& __t) _NOEXCEPT
{
    typedef typename std::remove_reference<_Tp>::type _Up;
    return static_cast<_Up&&>(__t);
}

template <class _Tp>
inline CONSTEXPR _Tp*
__constexpr_addressof(_Tp& __t)
{
    return ((_Tp*)&reinterpret_cast<const volatile char&>(__t));
}

#ifdef NDEBUG
# define ASSERTED_EXPRESSION(chk, expr) (expr)
#else
# define ASSERTED_EXPRESSION(chk, expr) ((chk) ? (expr) : (fail(#chk, __FILE__, __LINE__), (expr)))
static inline
void fail(const char* expr, const char* file, unsigned line)
{
# if defined(__clang__) || defined(__GNU_LIBRARY__)
    __assert(expr, file, line);
# elif defined(__GNUC__) || EPUB_PLATFORM(ANDROID)
    __assert(file, line, expr);
# else
//# warning I dont know how to fire assertion internals on this compiler.
# endif
}
#endif

template <typename _Tp>
struct __has_overloaded_addressof
{
    template <class _Obj>
    static CONSTEXPR FORCE_INLINE
    bool has_overload(...) { return false; }
    
    template <class _Obj, size_t _Sz = sizeof(std::declval<_Obj&>().operator&())>
    static CONSTEXPR FORCE_INLINE
    bool has_overload(bool) { return true; }
    
    CONSTEXPR static const bool value = has_overload<_Tp>(true);
};

template <typename _Tp, typename std::enable_if<!__has_overloaded_addressof<_Tp>::value, bool>::type = false>
CONSTEXPR FORCE_INLINE
_Tp* __static_addressof(_Tp& __t)
{
    return &__t;
}
template <typename _Tp, typename std::enable_if<__has_overloaded_addressof<_Tp>::value, bool>::type = false>
FORCE_INLINE
_Tp* __static_addressof(_Tp& __t)
{
#if __clang__
    return __builtin_addressof(__t);
#else
    return addressof(__t);
#endif
}

template <class _Up>
struct is_not_optional : public std::true_type {};

template <class _Tp>
struct is_not_optional<optional<_Tp>> : public std::false_type {};

struct __trivial_init_t {};
#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
constexpr __trivial_init_t __trivial_init{};
#else
extern __trivial_init_t __trivial_init;
#endif

struct in_place_t {};

#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
constexpr in_place_t in_place{};
#else
extern in_place_t in_place;
#endif

#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
struct nullopt_t
{
    nullopt_t() = delete;
    struct init {};
    constexpr nullopt_t(init) {};
};
constexpr nullopt_t nullopt{nullopt_t::init{}};
#else
struct nullopt_t
{
private:
    nullopt_t();
public:
	struct init {};
	FORCE_INLINE
    nullopt_t(init) {};
};
extern nullopt_t nullopt;
#endif

class bad_optional_access : public std::logic_error
{
public:
    explicit bad_optional_access(const std::string& what_arg) : logic_error{what_arg} {}
    explicit bad_optional_access(const char* what_arg) : logic_error{what_arg} {}
};

template <class _Tp>
union __optional_storage
{
    unsigned char   __dummy_;
    _Tp             __value_;
    
    CONSTEXPR FORCE_INLINE
    __optional_storage(__trivial_init_t) _NOEXCEPT
        : __dummy_()
        {}
    
    template <class... _Args>
    CONSTEXPR FORCE_INLINE
    __optional_storage(_Args&&... __args)
        : __value_(__constexpr_forward<_Args>(__args)...)
        {}
    
    ~__optional_storage()
        {}
};

#if HAS_CONSTEXPR_OPTIONAL
template <class _Tp>
union __constexpr_optional_storage
{
    unsigned char   __dummy_;
    _Tp             __value_;
    
    CONSTEXPR FORCE_INLINE
    __constexpr_optional_storage(__trivial_init_t) _NOEXCEPT
        : __dummy_()
        {}
    
    template <class... _Args>
    CONSTEXPR FORCE_INLINE
    __constexpr_optional_storage(_Args&&... __args)
        : __value_(__constexpr_forward<_Args>(__args)...)
        {}
    
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    ~__constexpr_optional_storage() = default;
#else
    ~__constexpr_optional_storage() {}
#endif
};
#endif

struct __only_set_initialized_t {};
#if EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
constexpr __only_set_initialized_t __only_set_initialized{};
#else
extern __only_set_initialized_t only_set_initialized;
#endif

template <class _Tp>
struct __optional_base
{
    bool                    __inited_;
    __optional_storage<_Tp> __storage_;
    
    CONSTEXPR FORCE_INLINE
    __optional_base() _NOEXCEPT
        : __inited_(false), __storage_(__trivial_init)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __optional_base(__only_set_initialized_t, bool __init) _NOEXCEPT
        : __inited_(__init), __storage_(__trivial_init)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __optional_base(const _Tp& __v)
        : __inited_(true), __storage_(__v)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __optional_base(_Tp&& __v)
        : __inited_(true), __storage_(__constexpr_move(__v))
        {}
    
    template <class ..._Args>
    FORCE_INLINE
    explicit
    __optional_base(in_place_t, _Args&&... __args)
        : __inited_(true), __storage_(__constexpr_forward<_Args>(__args)...)
        {}
    
    template <class _Up, class ..._Args,
                class = typename std::enable_if
                <
                    std::is_constructible<_Tp, std::initializer_list<_Tp>>::value,
                    bool
                >::type
             >
    FORCE_INLINE
    explicit
    __optional_base(in_place_t, std::initializer_list<_Up> __il, _Args&&... __args)
        : __inited_(true), __storage_(__il, std::forward<_Args>(__args)...)
        {}
    
    ~__optional_base()
        {
            if (__inited_)
                __storage_.__value_._Tp::~_Tp();
        }
    
};

#if HAS_CONSTEXPR_OPTIONAL
template <class _Tp>
struct __constexpr_optional_base
{
    bool                                __inited_;
    __constexpr_optional_storage<_Tp>   __storage_;
    
    CONSTEXPR FORCE_INLINE
    __constexpr_optional_base() _NOEXCEPT
        : __inited_(false), __storage_(__trivial_init)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __constexpr_optional_base(__only_set_initialized_t, bool __init) _NOEXCEPT
        : __inited_(__init), __storage_(__trivial_init)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __constexpr_optional_base(const _Tp& __v)
        : __inited_(true), __storage_(__v)
        {}
    
    CONSTEXPR FORCE_INLINE
    explicit
    __constexpr_optional_base(_Tp&& __v)
        : __inited_(true), __storage_(__constexpr_move(__v))
        {}
    
    template <class ..._Args>
    FORCE_INLINE
    explicit
    __constexpr_optional_base(in_place_t, _Args&&... __args)
        : __inited_(true), __storage_(__constexpr_forward<_Args>(__args)...)
        {}
    
    template <class _Up, class ..._Args,
                class = typename std::enable_if
                <
                    std::is_constructible<_Tp, std::initializer_list<_Tp>>::value
                >::type
             >
    FORCE_INLINE
    explicit
    __constexpr_optional_base(in_place_t, std::initializer_list<_Up> __il, _Args&&... __args)
        : __inited_(true), __storage_(__il, std::forward<_Args>(__args)...)
        {}
    
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    ~__constexpr_optional_base() = default;
#else
    ~__constexpr_optional_base() {}
#endif
    
};

template <class _Tp>
using __optional_base_impl = typename std::conditional
    <
        std::is_trivially_destructible<_Tp>::value,
        __constexpr_optional_base<_Tp>,
        __optional_base<_Tp>
    >::type;
#else
template <class _Tp> class __optional_base_impl : __optional_base<_Tp> {};
#endif // HAS_CONSTEXPR_OPTIONAL

template <class _Tp>
class optional : private __optional_base_impl<_Tp>
{
    static_assert(!std::is_same<typename std::decay<_Tp>::type, nullopt_t>::value, "optional<nullopt_t> is invalid");
    static_assert(!std::is_same<typename std::decay<_Tp>::type, in_place_t>::value, "optional<in_place_t> is invalid");
    
    typedef __optional_base_impl<_Tp>   _Base;
    
    CONSTEXPR FORCE_INLINE
    bool __initialized() const _NOEXCEPT
        { return _Base::__inited_; }

    FORCE_INLINE
    _Tp* __ptr()
        { return std::addressof(_Base::__storage_.__value_); }
    CONSTEXPR FORCE_INLINE
    const _Tp* __ptr() const
        { return __static_addressof(_Base::__storage_.__value_); }
    
#if EPUB_COMPILER_SUPPORTS(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    CONSTEXPR FORCE_INLINE
    const _Tp& __val() const &
        { return _Base::__storage_.__value_; }
    FORCE_INLINE
    _Tp& __val() &
        { return _Base::__storage_.__value_; }
    FORCE_INLINE
    _Tp&& __val() &&
        { return std::move(_Base::__storage_.__value_); }
#else
    CONSTEXPR FORCE_INLINE
    const _Tp& __val() const
        { return _Base::__storage_.__value_; }
    FORCE_INLINE
    _Tp& __val()
        { return _Base::__storage_.__value_; }
#endif
    
    FORCE_INLINE
    void __clear() _NOEXCEPT
        {
            if (__initialized())
                __ptr()->_Tp::~_Tp();
            _Base::__inited_ = false;
        }
    
    template <class... _Args>
    FORCE_INLINE
    void __initialize(_Args&&... __args)
        _NOEXCEPT_(_NOEXCEPT_(_Tp(std::forward<_Args>(__args)...)))
        {
            assert(!_Base::__inited_);
            new (__ptr()) _Tp(std::forward<_Args>(__args)...);
            _Base::__inited_ = true;
        }
    
    template <class _Up, class ..._Args>
    FORCE_INLINE
    void __initialize(std::initializer_list<_Up> __il, _Args&&... __args)
        _NOEXCEPT_(_NOEXCEPT_(_Tp(__il, std::forward<_Args>(__args)...)))
        {
            assert(!_Base::__inited_);
            new (__ptr()) _Tp(__il, std::forward<_Args>(__args)...);
            _Base::__inited_ = true;
        }
    
public:
    typedef _Tp                     value_type;
    
    // X.Y.4.1, constructors
    CONSTEXPR FORCE_INLINE
    optional() _NOEXCEPT
        : _Base()
        {}
    
    CONSTEXPR FORCE_INLINE
    optional(nullopt_t) _NOEXCEPT
        : _Base()
        {}
    
    FORCE_INLINE
    optional(const optional& __rhs)
        : _Base(__only_set_initialized, __rhs.__initialized())
        {
            if (__rhs.__initialized())
                new (__ptr()) _Tp(*__rhs);
        }
    
    FORCE_INLINE
    optional(optional&& __rhs)
        : _Base(__only_set_initialized, __rhs.__initialized())
        {
            if (__rhs.__initialized())
                new (__ptr()) _Tp(std::move(*__rhs));
        }
    
    CONSTEXPR FORCE_INLINE
    optional(const _Tp& __v)
        : _Base(__v)
        {}
    
    CONSTEXPR FORCE_INLINE
    optional(_Tp&& __v)
        : _Base(__constexpr_move(__v))
        {}
    
    template <class ..._Args>
    CONSTEXPR FORCE_INLINE
    explicit
    optional(in_place_t, _Args&&... __args)
        : _Base(in_place, __constexpr_forward<_Args>(__args)...)
        {}
    
    template <class _Up, class ..._Args,
                class = typename std::enable_if
                <
                    std::is_constructible<_Tp, std::initializer_list<_Up>, _Args...>::value,
                    bool
                >::type
             >
    CONSTEXPR FORCE_INLINE
    explicit
    optional(in_place_t, std::initializer_list<_Up> __il, _Args&&... __args)
        : _Base(in_place, __il, __constexpr_forward<_Args>(__args)...)
        {}
    
    // X.Y.4.2, destructor
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULTED_FUNCTIONS)
    ~optional() = default;
#else
    ~optional() {}
#endif
    
    // X.Y.4.3, assignment
    optional& operator=(const optional& __rhs)
        {
            if (__initialized() && !__rhs.__initialized()) {
                __clear();
            } else if (!__initialized() && __rhs.__initialized()) {
                __initialize(*__rhs);
            } else if (__initialized() && __rhs.__initialized()) {
                __val() = *__rhs;
            }
            return *this;
        }
    
    optional& operator=(optional&& __rhs) _NOEXCEPT_(std::is_nothrow_move_assignable<_Tp>::value)
        {
            if (__initialized() && !__rhs.__initialized()) {
                __clear();
            } else if (!__initialized() && __rhs.__initialized()) {
                __initialize(std::move(*__rhs));
            } else if (__initialized() && __rhs.__initialized()) {
                __val() = std::move(*__rhs);
            }
            return *this;
        }
    
#if EPUB_COMPILER_SUPPORTS(CXX_TRAILING_RETURN)
    template <class _Up>
    FORCE_INLINE
    auto operator=(_Up&& __rhs)
        -> typename std::enable_if
                    <
                        std::is_same<typename std::remove_reference<_Up>::type, _Tp>::value,
                        optional&
                    >::type
#else
    template <class _Up>
    FORCE_INLINE
    typename std::enable_if
             <
                 std::is_same<typename std::remove_reference<_Up>::type, _Tp>::value,
                 optional&
             >::type
    operator=(_Up&& __rhs)
#endif
        {
            if (__initialized()) {
                __val() = std::forward<_Up>(__rhs);
            } else {
                __initialize(std::forward<_Up>(__rhs));
            }
            return *this;
        }
    
    template <class ..._Args>
    FORCE_INLINE
    void emplace(_Args&&... __args)
        {
            __clear();
            __initialize(std::forward<_Args>(__args)...);
        }
    
    template <class _Up, class ..._Args>
    FORCE_INLINE
    void emplace(std::initializer_list<_Up> __il, _Args&&... __args)
        {
            __clear();
            __initialize<_Up, _Args...>(__il, std::forward<_Args>(__args)...);
        }
    
    // X.Y.4.4, swap
    FORCE_INLINE
    void swap(optional& __rhs)
        _NOEXCEPT_(std::is_nothrow_move_constructible<_Tp>::value &&
                   _NOEXCEPT_(std::swap(std::declval<_Tp&>(), std::declval<_Tp&>())))
        {
            if (__initialized() && !__rhs.__initialized()) {
                __rhs.__initialize(std::move(**this));
                __clear();
            } else if (!__initialized() && __rhs.__initialized()) {
                __initialize(std::move(*__rhs));
                __rhs.__clear();
            } else {
                using std::swap;
                swap(**this, *__rhs);
            }
        }
    
    // X.Y.4.5, observers
    CONSTEXPR FORCE_INLINE
    _Tp const* operator ->() const
        {
            return ASSERTED_EXPRESSION(__initialized(), __ptr());
        }
    
    FORCE_INLINE
    _Tp* operator ->()
        {
            assert(__initialized());
            return __ptr();
        }
    
    CONSTEXPR FORCE_INLINE
    _Tp const& operator *() const
        {
            return ASSERTED_EXPRESSION(__initialized(), __val());
        }
    
    FORCE_INLINE
    _Tp& operator *()
        {
            assert(__initialized());
            return __val();
        }
    
    CONSTEXPR FORCE_INLINE
    explicit
    operator bool() const _NOEXCEPT
        { return __initialized(); }
    
    CONSTEXPR FORCE_INLINE
    _Tp const& value() const
        {
            return (__initialized()
                    ? __val()
                    : throw bad_optional_access("bad optional access"));
        }
    
    FORCE_INLINE
    _Tp& value()
        {
            return (__initialized()
                    ? __val()
                    : (throw bad_optional_access("bad optional access"), __val()));
        }
    
#if EPUB_COMPILER_SUPPORTS(CXX_REFERENCE_QUALIFIED_FUNCTIONS)
    template <class _Up>
    CONSTEXPR FORCE_INLINE
    _Tp value_or(_Up&& __v) const &
        {
            return (bool(*this)
                    ? **this
                    : static_cast<_Tp>(__constexpr_forward<_Up>(__v)));
        }
    
    template <class _Up>
    FORCE_INLINE
    _Tp value_or(_Up&& __v) &&
        {
            return (bool(*this)
                    ? std::move(const_cast<optional<_Tp>&>(*this).__val())
                    : static_cast<_Tp>(std::forward<_Tp>(__v)));
        }
#else
    template <class _Up>
    CONSTEXPR FORCE_INLINE
    _Tp value_or(_Up&& __v) const
    {
        return (bool(*this)
                ? **this
                : static_cast<_Tp>(__constexpr_forward<_Up>(__v)));
    }
    
    template <class _Up>
    FORCE_INLINE
    _Tp value_or(_Up&& __v)
    {
        return (bool(*this)
                ? **this
                : static_cast<_Tp>(std::forward<_Tp>(__v)));
    }
#endif

};

// optionals holding lvalue and rvalue reference types are disallowed
template <class _Tp>
class optional<_Tp&>
{
    static_assert(sizeof(_Tp) == 0, "optional lvalue references are disallowed");
};

template <class _Tp>
class optional<_Tp&&>
{
    static_assert(sizeof(_Tp) == 0, "optional rvalue references are disallowed");
};

// X.Y.8, Relational operators
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator==(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return (bool(__x) != bool(__y)
            ? false
            : (bool(__x) == false
               ? true
               : *__x == *__y));
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator!=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__x == __y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(bool(__y)
             ? false
             : (!bool(__x)
                ? true
                : *__x < *__y));
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return (__y < __x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__y < __x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>=(const optional<_Tp>& __x, const optional<_Tp>& __y)
{
    return !(__x < __y);
}

// X.Y.9, Comparison with nullopt
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator==(const optional<_Tp>& __x, nullopt_t) _NOEXCEPT
{
    return !bool(__x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator==(nullopt_t, const optional<_Tp>& __y) _NOEXCEPT
{
    return !bool(__y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator!=(const optional<_Tp>& __x, nullopt_t) _NOEXCEPT
{
    return bool(__x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator!=(nullopt_t, const optional<_Tp>& __y) _NOEXCEPT
{
    return bool(__y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<(const optional<_Tp>&, nullopt_t) _NOEXCEPT
{
    return false;
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<(nullopt_t, const optional<_Tp>& __y) _NOEXCEPT
{
    return bool(__y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<=(const optional<_Tp>& __x, nullopt_t) _NOEXCEPT
{
    return !bool(__x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<=(nullopt_t, const optional<_Tp>&) _NOEXCEPT
{
    return true;
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>(const optional<_Tp>& __x, nullopt_t) _NOEXCEPT
{
    return bool(__x);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>(nullopt_t, const optional<_Tp>&) _NOEXCEPT
{
    return false;
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>=(const optional<_Tp>&, nullopt_t) _NOEXCEPT
{
    return true;
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>=(nullopt_t, const optional<_Tp>& __y) _NOEXCEPT
{
    return !bool(__y);
}

// X.Y.10, Comparison with T
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator==(const optional<_Tp>& __x, const _Tp& __y)
{
    return (bool(__x)
            ? *__x == __y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator==(const _Tp& __x, const optional<_Tp>& __y)
{
    return (bool(__y)
            ? __x = *__y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator!=(const optional<_Tp>& __x, const _Tp& __y)
{
    return !(__x == __y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator!=(const _Tp& __x, const optional<_Tp>& __y)
{
    return !(__x == __y);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<(const optional<_Tp>& __x, const _Tp& __y)
{
    return (bool(__x)
            ? *__x < __y
            : true);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<(const _Tp& __x, const optional<_Tp>& __y)
{
    return (bool(__y)
            ? __x < *__y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<=(const optional<_Tp>& __x, const _Tp& __y)
{
    return (bool(__x)
            ? *__x <= __y
            : true);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator<=(const _Tp& __x, const optional<_Tp>& __y)
{
    return (bool(__y)
            ? __x <= *__y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>(const optional<_Tp>& __x, const _Tp& __y)
{
    return (bool(__x)
            ? *__x > __y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>(const _Tp& __x, const optional<_Tp>& __y)
{
    return (bool(__y)
            ? __x > *__y
            : true);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>=(const optional<_Tp>& __x, const _Tp& __y)
{
    return (bool(__x)
            ? *__x >= __y
            : false);
}
template <class _Tp>
CONSTEXPR FORCE_INLINE
bool operator>=(const _Tp& __x, const optional<_Tp>& __y)
{
    return (bool(__y)
            ? __x >= *__y
            : true);
}

// X.Y.11, Specialized algorithms
template <class _Tp>
FORCE_INLINE
void swap(optional<_Tp>& __x, optional<_Tp>& __y) _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Tp>
CONSTEXPR FORCE_INLINE
optional<typename std::decay<_Tp>::type>
make_optional(_Tp&& __v)
{
    return optional<typename std::decay<_Tp>::type>(__constexpr_forward<_Tp>(__v));
}

EPUB3_END_NAMESPACE

namespace std {

// X.Y.12, hash support
template <class _Tp>
struct hash<EPUB3_NAMESPACE::optional<_Tp>>
{
    typedef typename hash<_Tp>::result_type result_type;
    typedef EPUB3_NAMESPACE::optional<_Tp>  argument_type;
    
    CONSTEXPR FORCE_INLINE
    result_type operator()(argument_type const& __arg) const
    {
        return (__arg
                ? hash<_Tp>{}(*__arg)
                : result_type{});
    }
};

}   // namespace std

#endif
