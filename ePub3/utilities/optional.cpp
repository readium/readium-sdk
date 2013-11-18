//
//  optional.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "optional.h"
#include "future.h"

#if !EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
// well hello there Microsoft!

EPUB3_BEGIN_NAMESPACE

__trivial_init_t __trivial_init();
in_place_t in_place();
nullopt_t nullopt(nullopt_t::init);

EPUB3_END_NAMESPACE

#endif
