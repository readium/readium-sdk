//
//  content_module.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_content_module_h
#define ePub3_content_module_h

#include <ePub3/ePub3.h>
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
