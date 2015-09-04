//
//  initialization.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
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

#include "initialization.h"
#include <mutex>
#include <ePub3/archive.h>
#include <ePub3/filter_manager_impl.h>
#include <ePub3/font_obfuscation.h>
#include <ePub3/switch_preprocessor.h>
#include <ePub3/object_preprocessor.h>
#include <ePub3/PassThroughFilter.h>

EPUB3_BEGIN_NAMESPACE

void InitializeSdk()
{
    static std::once_flag __once;
    std::call_once(__once, []{
        Archive::Initialize();
        FilterManager::Instance()->SetInstance(new FilterManagerImpl());
    });
}

void PopulateFilterManager()
{
    static std::once_flag __once;
    std::call_once(__once, []{
        FontObfuscator::Register();
		// If you want to activate the PassThroughFilter (to do testing or debugging),
		// simply uncomment the line below. Also take a look at the file PassThroughFilter.cpp
		// to see if the class is enabling itself.
		//
        // PassThroughFilter::Register();
        SwitchPreprocessor::Register();
        ObjectPreprocessor::Register();
    });
}

SDKInitializeAndRelease g_instance;

SDKInitializeAndRelease::SDKInitializeAndRelease()
{
    if (this != &g_instance)
        throw std::logic_error("SDKInitializeAndRelease should be a global static");

    Initialize();
}
SDKInitializeAndRelease::~SDKInitializeAndRelease()
{
    Teardown();
}
void SDKInitializeAndRelease::Initialize()
{
#if EPUB_USE(LIBXML2)
    xmlInitParser();	// Please check this invocation before any libxml2 parsing
#endif
}
void SDKInitializeAndRelease::Teardown()
{
#if EPUB_USE(LIBXML2)
    // One should call xmlCleanupParser() only when the process has finished using the library and all XML/HTML documents built with it.
    xmlCleanupParser();
#endif
}
SDKInitializeAndRelease& SDKInitializeAndRelease::Instance()
{
    return g_instance;  // returns the global static instance (for memory debugging purposes as the example)
}

EPUB3_END_NAMESPACE
