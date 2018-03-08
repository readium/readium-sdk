//
//  content_module.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ePub3_content_module_h
#define ePub3_content_module_h

#include <ePub3/epub3.h>
#include <ePub3/user_action.h>
#include <ePub3/utilities/future.h>
#include <memory>

EPUB3_BEGIN_NAMESPACE

#if FUTURE_ENABLED

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
template <typename _Tp>
using async_result = future<_Tp>;

template <typename _Tp>
using promised_result = promise<_Tp>;
#else
# define async_result std::future
# define promised_result std::promise
#endif

#endif //FUTURE_ENABLED

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
#if FUTURE_ENABLED
    virtual
    async_result<ContainerPtr>
    ProcessFile(const string& path,
                launch policy=launch::any)                  = 0;
#else
    virtual
    ContainerPtr
    ProcessFile(const string& path)                  = 0;
#endif //FUTURE_ENABLED
    //////////////////////////////////////////////
    // Content Filters

    virtual
    void
    RegisterContentFilters()                                = 0;

#if FUTURE_ENABLED
    //////////////////////////////////////////////
    // User actions

    virtual
    async_result<bool>
    ApproveUserAction(const UserAction& action)             = 0;
#endif //FUTURE_ENABLED

    virtual string GetModuleName()                          = 0;

};

typedef std::shared_ptr<ContentModule> ContentModulePtr;

EPUB3_END_NAMESPACE

#endif
