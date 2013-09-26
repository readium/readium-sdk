//
//  signatures.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
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

#include "signatures.h"

EPUB3_BEGIN_NAMESPACE

DigitalSignature::DigitalSignature(shared_ptr<xml::Node> signatureNode)
{
    
}
DigitalSignature& DigitalSignature::operator=(DigitalSignature&& o)
{
    _signedInfo = std::move(o._signedInfo);
    _keyInfo = std::move(o._keyInfo);
    _object = std::move(o._object);
    return *this;
}

EPUB3_END_NAMESPACE
