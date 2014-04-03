//
//  content_module_manager.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_content_module_manager_h
#define ePub3_content_module_manager_h

#include <ePub3/epub3.h>
#include <ePub3/content_module.h>
#include <ePub3/credential_request.h>
#include <map>
#include <future>
#include <memory>

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
    RegisterContentModule(std::shared_ptr<ContentModule> module,
                          const string& name) _NOEXCEPT;
    
    ////////////////////////////////////////////////////
    // Services for DRM implementations
    
    static
    void
    DisplayMessage(const string& title,
                   const string& message) _NOEXCEPT;
    
    static
    async_result<Credentials>
    RequestCredentialInput(const CredentialRequest& request);
    
private:
    static std::unique_ptr<ContentModuleManager>        s_instance;
    
    std::mutex                                          _mutex;
    std::map<string, std::shared_ptr<ContentModule>>    _known_modules;
    
    friend class Container;
    
    // for the use of Container::OpenContainer(path)
    async_result<ContainerPtr> LoadContentAtPath(const string& path, launch policy);
    
};

EPUB3_END_NAMESPACE

#endif
