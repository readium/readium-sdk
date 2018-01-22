//
//  initialization.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
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

#include "initialization.h"
#if FUTURE_ENABLED
#include <mutex>
#endif //FUTURE_ENABLED
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

EPUB3_END_NAMESPACE

