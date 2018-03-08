//
//  encryption.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
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

#include "encryption.h"
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

#define IDPFEncURI "http://www.idpf.org/2016/encryption#compression"

bool EncryptionInfo::ParseXML(shared_ptr<xml::Node> node)
{
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(node->Document(), {{"enc", XMLENCNamespaceURI}, {"dsig", XMLDSigNamespaceURI} , {"ds", XMLDSigNamespaceURI}, { "ep", IDPFEncURI }});
#else
    XPathWrangler::NamespaceList nsList;
    nsList["enc"] = XMLENCNamespaceURI;
    nsList["dsig"] = XMLDSigNamespaceURI;
    nsList["ds"] = XMLDSigNamespaceURI;
    nsList["ep"] = IDPFEncURI;
    XPathWrangler xpath(node->doc, nsList);
#endif
    
    auto strings = xpath.Strings("./enc:EncryptionMethod/@Algorithm", node);
    if ( strings.empty() )
        return false;
    
    _algorithm = strings[0];

    strings = xpath.Strings("./ds:KeyInfo/ds:RetrievalMethod/@Type", node);
    if ( !strings.empty() ) {
        _keyRetrievalMethodType = strings[0];
    }
    
    strings = xpath.Strings("./enc:CipherData/enc:CipherReference/@URI", node);
    if ( strings.empty() )
        return false;
    
    _path = strings[0];
    
    strings = xpath.Strings("./enc:EncryptionProperties/enc:EncryptionProperty/ep:Compression/@Method", node);
    if (!strings.empty())
    {
        if (strings[0] == "0" || strings[0] == "8")
            _compression_method = strings[0];
        else
            return false;
        
    }
    else if  (strings.empty())
        _compression_method = "0";
    
    strings = xpath.Strings("./enc:EncryptionProperties/enc:EncryptionProperty/ep:Compression/@OriginalLength", node);
    if (!strings.empty())
    {
        for ( int i = 0 ; i < strings[0].size() ; i++)
            if (strings[0][i] < '0' || strings[0][i] > '9' )
                return false;
        
        _uncompressed_size = strings[0];
    }

    return true;
}

EPUB3_END_NAMESPACE
