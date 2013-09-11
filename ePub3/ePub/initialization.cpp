//
//  initialization.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "initialization.h"
#include <ePub3/archive.h>
#include <ePub3/filter_manager_impl.h>
#include <ePub3/font_obfuscation.h>
#include <ePub3/switch_preprocessor.h>
#include <ePub3/object_preprocessor.h>

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
        SwitchPreprocessor::Register();
        ObjectPreprocessor::Register();
    });
}

EPUB3_END_NAMESPACE

