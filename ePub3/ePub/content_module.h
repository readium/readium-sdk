//
//  content_module.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
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

#ifndef ePub3_content_module_h
#define ePub3_content_module_h

#include <ePub3/epub3.h>
#include <ePub3/user_action.h>
#include <ePub3/utilities/future.h>
#include <memory>

EPUB3_BEGIN_NAMESPACE

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
template <typename _Tp>
using async_result = future<_Tp>;

template <typename _Tp>
using promised_result = promise<_Tp>;
#else
# define async_result std::future
# define promised_result std::promise
#endif

class ContentModule : public std::enable_shared_from_this<ContentModule>
{
public:
    ContentModule() {}
    ContentModule(ContentModule&&) {}
    virtual ~ContentModule() {}
    
    virtual
    ContentModule& operator=(ContentModule&&) { return *this; }
    
private:
    ContentModule(const ContentModule&)                     _DELETED_;
    ContentModule& operator=(const ContentModule&)          _DELETED_;
    
public:
    //////////////////////////////////////////////
    // Token files
    
    virtual
    async_result<ContainerPtr>
    ProcessFile(const string& path,
                launch policy=launch::any)                  = 0;
    
    //////////////////////////////////////////////
    // Content Filters
    
    virtual
    void
    RegisterContentFilters()                                = 0;
    
    //////////////////////////////////////////////
    // User actions
    
    virtual
    async_result<bool>
    ApproveUserAction(const UserAction& action)             = 0;
    
};

EPUB3_END_NAMESPACE

#endif
