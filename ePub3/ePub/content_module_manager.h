//
//  content_module_manager.h
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

#ifndef ePub3_content_module_manager_h
#define ePub3_content_module_manager_h

#include <ePub3/epub3.h>
#include <ePub3/content_module.h>
#include <ePub3/credential_request.h>
#include <ePub3/utilities/utfstring.h>
#include <map>
#include <memory>

#if FUTURE_ENABLED
#include <mutex>
#include <future>
#endif //FUTURE_ENABLED

EPUB3_BEGIN_NAMESPACE

class ContentModuleManager
{
public:
    ContentModuleManager();
    virtual ~ContentModuleManager();
    
private:
    ContentModuleManager(const ContentModuleManager&)                   _DELETED_;
    ContentModuleManager(ContentModuleManager&&)                        _DELETED_;
    ContentModuleManager& operator=(const ContentModuleManager&)        _DELETED_;
    ContentModuleManager& operator=(ContentModuleManager&&)             _DELETED_;
    
public:
    ////////////////////////////////////////////////////
    // Accessing the singleton instance
    static
    ContentModuleManager*
    Instance() _NOEXCEPT;
    
    ////////////////////////////////////////////////////
    // Registering Content Module implementations
    
    void
    RegisterContentModule(ContentModule* module,
                          const ePub3::string& name) _NOEXCEPT;

    ////////////////////////////////////////////////////
    // Services for DRM implementations
#if FUTURE_ENABLED
    static
    void
    DisplayMessage(const string& title,
                   const string& message) _NOEXCEPT;

    static
    async_result<Credentials>
    RequestCredentialInput(const CredentialRequest& request);
#endif //FUTURE_ENABLED

private:
    static std::unique_ptr<ContentModuleManager>        s_instance;

#if FUTURE_ENABLED
std::mutex                                          _mutex;
#endif //FUTURE_ENABLED

    std::map<string, std::shared_ptr<ContentModule>>    _known_modules;
    
    friend class Container;

#if FUTURE_ENABLED
    async_result<ContainerPtr> LoadContentAtPath(const string& path, launch policy);
#else
    ContainerPtr LoadContentAtPath(const string& path);
#endif //FUTURE_ENABLED
};

EPUB3_END_NAMESPACE

#endif
