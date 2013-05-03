//
//  ref_counted.h
//  ePub3
//
//  Created by Jim Dovey on 2013-04-30.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#if BUILDING_EPUB3 || !EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR)
#include <ePub3/utilities/ref_counted.h>
const adopt_ref_t adopt_ref = {};
#endif