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

EPUB3_BEGIN_NAMESPACE

void InitializeSdk()
{
    Archive::Initialize();
    FilterManager::Instance()->SetInstance(new FilterManagerImpl());
}

void PopulateFilterManager()
{
    FontObfuscator::Register();
}

EPUB3_END_NAMESPACE

