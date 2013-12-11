//
//  optional.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "optional.h"

#ifdef _LIBCPP_HAS_NO_CONSTEXPR
// well hello there Microsoft!

OPEN_EXPERIMENTAL

__trivial_init_t __trivial_init{};
in_place_t in_place{};
nullopt_t nullopt = nullopt_t(nullopt_t::init{});

CLOSE_EXPERIMENTAL

#endif
