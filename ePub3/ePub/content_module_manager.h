//
//  content_module_manager.h
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
