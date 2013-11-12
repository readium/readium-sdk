//
//  content_module_manager.cpp
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "content_module_manager.h"
#include "content_module.h"
#include "user_action.h"
#include "credential_request.h"
#include <thread>

EPUB3_BEGIN_NAMESPACE

std::unique_ptr<ContentModuleManager> ContentModuleManager::s_instance;

ContentModuleManager::ContentModuleManager() : _mutex(), _known_modules()
{
}
ContentModuleManager::~ContentModuleManager()
{
}
ContentModuleManager* ContentModuleManager::Instance() _NOEXCEPT
{
    std::once_flag __once;
    std::call_once(__once, [](std::unique_ptr<ContentModuleManager> *ptr) {
        ptr->reset(new ContentModuleManager);
    }, &s_instance);
    return s_instance.get();
}
void ContentModuleManager::RegisterContentModule(std::shared_ptr<ContentModule> module,
                                                 const string& name) _NOEXCEPT
{
    std::unique_lock<std::mutex>(_mutex);
    _known_modules[name] = module;
}

void ContentModuleManager::DisplayMessage(const string& title, const string& message) _NOEXCEPT
{
    // nothing at the moment...
}
std::future<Credentials>
ContentModuleManager::RequestCredentialInput(const CredentialRequest &request)
{
    // nothing yet...
    // really want the std::make_immediate_future<>() function right now...
    std::promise<Credentials> promise;
    
    Credentials none;
    promise.set_value(std::move(none));
    return promise.get_future();
}

std::future<ContainerPtr>
ContentModuleManager::LoadContentAtPath(const string& path, std::launch policy)
{
    std::unique_lock<std::mutex>(_mutex);
    
    if (_known_modules.empty())
    {
        // special case for when we don't have any Content Modules to rely on for an initialized result
        std::promise<ContainerPtr> promise;
        promise.set_value(nullptr);
        return promise.get_future();
    }
    
    std::future<ContainerPtr> result;
    for (auto& item : _known_modules)
    {
        auto modulePtr = item.second;
        result = modulePtr->ProcessFile(path, policy);
        
        // check the state of the future -- has it already been set?
        std::future_status status = result.wait_for(std::chrono::system_clock::duration(0));
        
        // if it's deferred, then the computation will only happen when the value is requested
        // if it's ready, the call to get() will never block
        if (status == std::future_status::deferred || status == std::future_status::ready) {
            if (bool(result.get())) {
                // we have a valid container already
//                result.then([modulePtr]() {
                    modulePtr->RegisterContentFilters();
//                });
                break;
            } else {
                continue;       // no container, so try the next module
            }
        } else {
            // it must be 'timeout', which means the module is attempting to process the file
            // we take this to mean that we stop looking and return the result
//            result.then([modulePtr]() {
                modulePtr->RegisterContentFilters();
//            });
            break;
        }
    }
    
    return result;
}

EPUB3_END_NAMESPACE
