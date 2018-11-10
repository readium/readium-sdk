//
//  initialization.h
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
