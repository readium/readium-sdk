//
//  xml_identifiable.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-10.
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
