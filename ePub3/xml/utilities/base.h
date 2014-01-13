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

#include <ePub3/base.h>

#define EPUB3_XML_BEGIN_NAMESPACE EPUB3_BEGIN_NAMESPACE namespace xml {
#define EPUB3_XML_END_NAMESPACE } EPUB3_END_NAMESPACE

#include <exception>
#include <string>
#include <map>
#include <memory>

#if EPUB_USE(LIBXML2)
#include <libxml/xmlerror.h>
#define xml_native_cast reinterpret_cast
#else
struct xmlError {
	char* message;
};
typedef struct xmlError* xmlErrorPtr;
#endif

#if EPUB_USE(WIN_XML) || EPUB_USE(WIN_PHONE_XML)
#define xml_native_cast dynamic_cast
#define __winstr(x) dynamic_cast<::Platform::String^>(x)
#else
#define xml_native_cast reinterpret_cast
#endif

EPUB3_XML_BEGIN_NAMESPACE

#if !EPUB_PLATFORM(WINRT)
// generic 'get me a wrapper' template
/**
 @ingroup xml-utils
 */
template <class _Tp, typename _Nm>
static inline std::shared_ptr<_Tp> Wrapped(_Nm * __n)
{
    typedef typename std::shared_ptr<_Tp> _Ret;
    
    if ( __n == nullptr )
        return nullptr;
    
    // _private is set to a heap-based std::shared_ptr -- return the value, not the pointer
    if ( __n->_private != nullptr )
        return *(reinterpret_cast<_Ret*>(__n->_private));
    
    _Ret* __r = new _Ret(new _Tp(__n));
    __n->_private = __r;
    return *__r;
}
#endif
/**
 @ingroup xml-utils
 */
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
#if EPUB_COMPILER_SUPPORTS(CXX_DELEGATING_CONSTRUCTORS)
    exception(const char * s, xmlErrorPtr err = NULL) throw () : exception(std::string(s), err) {}
#else
	exception(const char * s, xmlErrorPtr err = NULL) _NOEXCEPT
	{
		if ( err == NULL ) {
			message = s;
		} else {
			message = s;
			message += ":\n";
			message += err->message;
		}
	}
#endif
    virtual ~exception() throw () {}
    
    virtual const char * what() const throw () { return message.c_str(); }
    
protected:
    std::string message;
};

/**
 @ingroup xml-utils
 */
class ParserError : public exception {
public:
    ParserError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    ParserError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~ParserError() throw () {}
};
/**
 @ingroup xml-utils
 */
class ValidationError : public exception  {
public:
    ValidationError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    ValidationError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~ValidationError() throw () {}
};
/**
 @ingroup xml-utils
 */
class InternalError : public exception  {
public:
    InternalError(const std::string & context, xmlErrorPtr err = NULL) throw () : exception(context.c_str(), err) {}
    InternalError(const char * s, xmlErrorPtr err = NULL) throw () : exception(s, err) {}
    virtual ~InternalError() throw () {}
};

// note that MOVE is allowed, just not COPY
/**
 @ingroup xml-utils
 */
template <class _Tp>
class WrapperBase : public std::enable_shared_from_this<_Tp>
{
public:
    WrapperBase() {}
    WrapperBase(WrapperBase && moveRef) {}
    virtual ~WrapperBase() {}
    
    WrapperBase & operator = (WrapperBase && moveRef) { return *this; }

	template <class... _Args>
	static std::shared_ptr<_Tp> New(_Args&& ...__args) {
		return std::make_shared<_Tp>(std::forward<_Args>(__args)...);
	}
    
private:
    WrapperBase(WrapperBase & o);
    WrapperBase & operator = (WrapperBase & o);
};

#if EPUB_PLATFORM(WINRT)
#if EPUB_USE(WIN_PHONE_XML)
typedef ::PhoneSupportInterfaces::IXmlNode		_NodeObj;
typedef ::PhoneSupportInterfaces::IXmlElement	_ElementObj;
#else // EPUB_USE(WIN_XML)
typedef ::Windows::Data::Xml::Dom::IXmlNode		_NodeObj;
typedef ::Windows::Data::Xml::Dom::IXmlElement	_ElementObj;
#endif
static inline ::Platform::String^ GetAttributeValueRecursiveNS(_NodeObj^ node, ::Platform::String^ uri, ::Platform::String^ name)
{
	_ElementObj^ element = dynamic_cast<_ElementObj^>(node);
	if (element == nullptr)
		element = dynamic_cast<_ElementObj^>(node->ParentNode);

	while (element != nullptr)
	{
		auto attr = element->GetAttributeNS(uri, name);
		if (attr != nullptr)
			return attr;
		element = dynamic_cast<_ElementObj^>(element->ParentNode);
	}

	return nullptr;
}
static inline ::Platform::String^ GetAttributeValueRecursive(_NodeObj^ node, ::Platform::String^ name)
{
	_ElementObj^ element = dynamic_cast<_ElementObj^>(node);
	if (element == nullptr)
		element = dynamic_cast<_ElementObj^>(node->ParentNode);

	while (element != nullptr)
	{
		auto attr = element->GetAttribute(name);
		if (attr != nullptr)
			return attr;
		element = dynamic_cast<_ElementObj^>(element->ParentNode);
	}

	return nullptr;
}
#endif

EPUB3_XML_END_NAMESPACE

#endif /* ePub3_xml_base_h */
