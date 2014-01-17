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
#include <ePub3/container.h>

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
    static std::once_flag __once;
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
async_result<Credentials>
ContentModuleManager::RequestCredentialInput(const CredentialRequest &request)
{
    // nothing yet...
    // really want the std::make_immediate_future<>() function right now...
    promised_result<Credentials> promise;
    
    Credentials none;
#if EPUB_PLATFORM(WIN_PHONE)
	promise.set(std::move(none));
	return async_result<Credentials>(promise);
#else
	promise.set_value(std::move(none));
	return promise.get_future();
#endif
}

async_result<ContainerPtr>
ContentModuleManager::LoadContentAtPath(const string& path, launch policy)
{
    std::unique_lock<std::mutex>(_mutex);
    
    if (_known_modules.empty())
    {
        // special case for when we don't have any Content Modules to rely on for an initialized result
        promised_result<ContainerPtr> promise;
#if EPUB_PLATFORM(WIN_PHONE)
		promise.set(nullptr);
		return async_result<ContainerPtr>(promise);
#else
        promise.set_value(nullptr);
        return promise.get_future();
#endif
    }
    
    async_result<ContainerPtr> result;
    for (auto& item : _known_modules)
    {
        auto modulePtr = item.second;
        result = modulePtr->ProcessFile(path, policy);
        
        // check the state of the future -- has it already been set?
		// if it's ready, the call to get() will never block
        if (__ar_has_value(result)) {
			// unpack the future
			ContainerPtr container = result.get();

            if (bool(container)) {
                // we have a valid container already
				promised_result<ContainerPtr> p;
#if EPUB_PLATFORM(WIN_PHONE)
				p.set(container);
				result = async_result<ContainerPtr>(p);
#else
				p.set_value(container);
				result = p.get_future();
#endif
//				result = make_ready_future(container);
//                result.then([modulePtr]() {
                    modulePtr->RegisterContentFilters();
//                });
                break;
            } else {
                continue;       // no container, so try the next module
            }
        } else {
            // it must be 'timeout' or 'deferred', which means the module is attempting to process the file
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
