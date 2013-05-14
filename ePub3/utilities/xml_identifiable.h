//
//  xml_identifiable.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-10.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_xml_identifiable_h
#define ePub3_xml_identifiable_h

#include <ePub3/utilities/utfstring.h>

EPUB3_BEGIN_NAMESPACE

class XMLIdentifiable
{
private:
    string          _xmlID;
    
public:
                    XMLIdentifiable()                           { }
                    XMLIdentifiable(const XMLIdentifiable& o) : _xmlID(o._xmlID) { }
                    XMLIdentifiable(XMLIdentifiable&& o) : _xmlID(std::move(o._xmlID)) { }
    virtual         ~XMLIdentifiable()                          { }
    
    const string&   XMLIdentifier()                     const   { return _xmlID; }
    void            SetXMLIdentifier(const string& str)         { _xmlID = str; }
    
};

EPUB3_END_NAMESPACE

#endif
