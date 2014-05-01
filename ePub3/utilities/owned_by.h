//
//  owned_by.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-03.
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

#ifndef __ePub3__owned_by__
#define __ePub3__owned_by__

#include <memory>
#include <type_traits>

#if EPUB_COMPILER_SUPPORTS(CXX_STATIC_ASSERT)
# include <ePub3/utilities/pointer_type.h>
#endif

EPUB3_BEGIN_NAMESPACE

template <class _Tp>
class OwnedBy
{
    // _Tp must be a subclass of PointerType -- hard to assert this though (what template arg? include header, etc?)
//#if EPUB_COMPILER_SUPPORTS(CXX_STATIC_ASSERT)
//    static_assert(std::is_base_of<PointerType<_Tp>, _Tp>::value, "OwnedBy's template parameter must be a subclass of PointerType");
//#endif
    
protected:
    typedef std::shared_ptr<typename std::remove_pointer<_Tp>::type> shared_type;
    typedef std::weak_ptr<typename std::remove_pointer<_Tp>::type>   weak_type;
    
private:
    weak_type       _owner;
    
protected:
    OwnedBy(const shared_type& __owner) : _owner(__owner) {}
    OwnedBy(const OwnedBy& __o) : _owner(__o._owner) {}
    OwnedBy(OwnedBy&& __o) : _owner(std::move(__o._owner)) {}
    virtual ~OwnedBy() {}
    
    OwnedBy& operator=(const OwnedBy& __o) { _owner = __o._owner; return *this; }
    OwnedBy& operator=(OwnedBy&& __o) { _owner = std::move(__o._owner); return *this; }
    
public:
    shared_type Owner() const { return _owner.lock(); }
    bool Orphaned() const { return _owner.expired(); }
    
    void SetOwner(const shared_type __owner) _NOEXCEPT { _owner = __owner; }
#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULT_TEMPLATE_ARGS)
    template <class _Yp,
              class = typename std::enable_if
                      <
                        std::is_convertible<_Yp*, _Tp*>::value
                      >::type
             >
    void SetOwner(const std::shared_ptr<_Yp> __owner) _NOEXCEPT { _owner = __owner; }
#else
public:
    template <class _Yp>
    void SetOwner(const std::shared_ptr<_Yp> __owner,
                  typename std::enable_if
                  <
                    std::is_convertible<_Yp*, _Tp*>::value
                  >::type ** = 0) _NOEXCEPT {
        _owner = __owner;
    }
#endif
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__owned_by__) */
