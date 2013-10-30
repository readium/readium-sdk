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
#include <future>

EPUB3_BEGIN_NAMESPACE

class ContentModule
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
    std::future<ContainerPtr>
    ProcessFile(const string& path,
                std::launch policy=std::launch::any)        = 0;
    
    //////////////////////////////////////////////
    // Content Filters
    
    virtual
    void
    RegisterContentFilters()                                = 0;
    
    //////////////////////////////////////////////
    // User actions
    
    virtual
    std::future<bool>
    ApproveUserAction(const UserAction& action)             = 0;
    
};

EPUB3_END_NAMESPACE

#endif
