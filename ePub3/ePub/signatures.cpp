//
//  signatures.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
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
