//
//  encryption.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
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
        for (XPathWrangler::StringList::size_type i = 0; i < strings[0].size(); i++)
            if (strings[0][i] < '0' || strings[0][i] > '9' )
                return false;
        
        _uncompressed_size = strings[0];
    }

    return true;
}

EPUB3_END_NAMESPACE
