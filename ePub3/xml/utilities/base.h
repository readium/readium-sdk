//
//  base.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-16.
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

#ifndef ePub3_xml_base_h
#define ePub3_xml_base_h

#include "../../base.h"
#include "../../utilities/basic.h"

#define EPUB3_XML_BEGIN_NAMESPACE EPUB3_BEGIN_NAMESPACE namespace xml {
#define EPUB3_XML_END_NAMESPACE } EPUB3_END_NAMESPACE

#include <exception>
#include <string>
#include <map>
#include <libxml/xmlerror.h>

EPUB3_XML_BEGIN_NAMESPACE

// generic 'get me a wrapper' template
template <class _T, typename _N>
static inline _T * Wrapped(_N * n)
{
    if ( n == nullptr ) return nullptr;
    if ( n->_private != nullptr ) return reinterpret_cast<_T*>(n->_private);
    return new _T(n);
}

class exception : public std::exception
{
public:
    exception(const std::string & context, xmlErrorPtr err = NULL) throw ()
    {
        if ( err == NULL ) {
            message = context;
        } else {
            message = context + ":\n" + err->message;
        }
    }
    exception(const char * s, xmlErrorPtr err = NULL) throw () : exception(std::string(s), err) {}
    virtual ~exception() throw () {}
    
    virtual const char * what() const throw () { return message.c_str(); }
    
protected:
    std::string message;
};

class ParserError : public exception {
public:
    ParserError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    ParserError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~ParserError() {}
};
class ValidationError : public exception  {
public:
    ValidationError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    ValidationError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~ValidationError() {}
};
class InternalError : public exception  {
public:
    InternalError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    InternalError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~InternalError() {}
};

// note that MOVE is allowed, just not COPY
class WrapperBase
{
public:
    WrapperBase() {}
    WrapperBase(WrapperBase && moveRef) {}
    virtual ~WrapperBase() {}
    
    WrapperBase & operator = (WrapperBase && moveRef) { return *this; }
    
private:
    WrapperBase(WrapperBase & o);
    WrapperBase & operator = (WrapperBase & o);
};



EPUB3_XML_END_NAMESPACE

#endif /* ePub3_xml_base_h */
