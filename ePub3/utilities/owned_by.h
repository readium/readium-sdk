//
//  owned_by.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-03.
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

#ifndef __ePub3__owned_by__
#define __ePub3__owned_by__

#include <memory>
#include <type_traits>

EPUB3_BEGIN_NAMESPACE

template <class _Tp>
class OwnedBy
{
    // _Tp must be a subclass of _Owner
    //static_assert(std::is_base_of<std::enable_shared_from_this, _Tp>::value, "OwnedBy's template parameter must be a subclass of std::enable_shared_from_this");
    
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
