//
//  initialization.h
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

#ifndef ePub3_initialization_h
#define ePub3_initialization_h

#include <ePub3/epub3.h>

EPUB3_BEGIN_NAMESPACE

EPUB3_EXPORT void InitializeSdk();

EPUB3_EXPORT void PopulateFilterManager();

// The static signleton object, to initialize and teardown the SDK
// TODO: Add your global initialization and release code into Initialize() and Teardown() methods respectively
class SDKInitializeAndRelease
{
private:
public:
    SDKInitializeAndRelease();
    ~SDKInitializeAndRelease();
    EPUB3_EXPORT void Initialize();  // helper function, can be called to initialize the SDK memory (i.e. in case of memory leak debugging)
    EPUB3_EXPORT void Teardown();    // helper function, can be called to release the SDK memory (i.e. in case of memory leak debugging)
    EPUB3_EXPORT static SDKInitializeAndRelease& Instance();
};


EPUB3_END_NAMESPACE

#endif
