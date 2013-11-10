//
//  media-overlays_smil_data.cpp
//  ePub3
//
//  Created by Daniel Weck on 2013-09-18.
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

#include "media-overlays_smil_data.h"

EPUB3_BEGIN_NAMESPACE

        const string SMILData::Sequence::_Name = string("seq");
        const string SMILData::Parallel::_Name = string("par");
        const string SMILData::Audio::_Name = string("audio");
        const string SMILData::Text::_Name = string("text");

        EPUB3_END_NAMESPACE
