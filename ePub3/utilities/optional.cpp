//
//  optional.cpp
//  ePub3
//
//  Created by Jim Dovey on 11/7/2013.
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

#include "optional.h"

#if !EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
// well hello there Microsoft!

EPUB3_BEGIN_NAMESPACE

__trivial_init_t __trivial_init{};
in_place_t in_place{};
nullopt_t nullopt = nullopt_t(nullopt_t::init{});

EPUB3_END_NAMESPACE

#endif
