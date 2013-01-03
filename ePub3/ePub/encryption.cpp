//
//  encryption.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "encryption.h"
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

EncryptionInfo::EncryptionInfo(xmlNodePtr node)
{
    XPathWrangler xpath(node->doc, {{"enc", XMLENCNamespaceURI}, {"dsig", XMLDSigNamespaceURI}});
    
    auto strings = xpath.Strings("./enc:EncryptionMethod/@Algorithm");
    if ( strings.empty() )
        throw std::invalid_argument("Node does not contain enc:EncryptionMethod with an Algorithm attribute");
    
    _algorithm = strings[0];
    
    strings = xpath.Strings("./enc:CipherData/enc:CipherReference/@URI");
    if ( strings.empty() )
        throw std::invalid_argument("Node does not specify a cipher reference URI");
    
    _path = strings[0];
}

EPUB3_END_NAMESPACE
