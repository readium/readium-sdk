//
//  signatures.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "signatures.h"

EPUB3_BEGIN_NAMESPACE

DigitalSignature::DigitalSignature(xmlNodePtr signatureNode)
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