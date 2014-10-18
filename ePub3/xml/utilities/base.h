//
//  base.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-16.
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

#ifndef ePub3_xml_base_h
#define ePub3_xml_base_h

#include <ePub3/base.h>

#define EPUB3_XML_BEGIN_NAMESPACE EPUB3_BEGIN_NAMESPACE namespace xml {
#define EPUB3_XML_END_NAMESPACE } EPUB3_END_NAMESPACE

#include <exception>
#include <string>
#include <map>
#include <memory>

#define PROMISCUOUS_LIBXML_OVERRIDES 0

#if EPUB_USE(LIBXML2)
#include <libxml/xmlerror.h>
#define xml_native_cast reinterpret_cast
#undef PROMISCUOUS_LIBXML_OVERRIDES
#define PROMISCUOUS_LIBXML_OVERRIDES 1
#else
struct xmlError {
	char* message;
};
typedef struct xmlError* xmlErrorPtr;
#endif

#if EPUB_USE(WIN_XML)
#define xml_native_cast dynamic_cast
#define __winstr(x) dynamic_cast<::Platform::String^>(x)
#else
#define xml_native_cast reinterpret_cast
#endif

EPUB3_XML_BEGIN_NAMESPACE

#if PROMISCUOUS_LIBXML_OVERRIDES == 0
#ifdef __cplusplus
                extern "C" {
#endif
                extern void __resetLibXMLOverrides(void);
                extern void __setupLibXML(void);
#ifdef __cplusplus
                }
#endif
#endif /* PROMISCUOUS_XML_OVERRIDES */

static CONSTEXPR const unsigned int _READIUM_XML_SIGNATURE = 0x52586D6C;    /* 'RXml' */

class Node;

template <class _Tp, typename _Nm>
static inline std::shared_ptr<_Tp> Wrapped(_Nm * __n);

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
	static inline FORCE_INLINE
    std::shared_ptr<_Tp>
    New(_Args&& ...__args)
        {
            return std::make_shared<_Tp>(std::forward<_Args>(__args)...);
        }
    
    template <class _Sub, typename... _Args>
    static inline FORCE_INLINE
    typename std::enable_if
        <
            std::is_base_of<_Tp, _Sub>::value,
            std::shared_ptr<_Sub>
        >::type
    New(_Args&& ...__args)
        {
            return std::make_shared<_Sub>(std::forward<_Args>(__args)...);
        }
    
    virtual
    void release() = 0;
    
private:
    WrapperBase(WrapperBase & o);
    WrapperBase & operator = (WrapperBase & o);
    
};

#if EPUB_USE(LIBXML2)
template <typename _Tp>
struct LibXML2Private
{
    LibXML2Private()
        : __sig(_READIUM_XML_SIGNATURE), __ptr(nullptr)
        {}
    LibXML2Private(_Tp* __p)
        : __sig(_READIUM_XML_SIGNATURE), __ptr(__p)
        {}
    LibXML2Private(std::shared_ptr<_Tp>& __p)
        : __sig(_READIUM_XML_SIGNATURE), __ptr(__p)
        {}
    ~LibXML2Private()
        { __sig = 0xbaadf00d; }
    
    // data member-- used to determine if this is a Readium-made pointer
    unsigned int __sig;
    std::shared_ptr<_Tp> __ptr;
};
#endif

#if !EPUB_PLATFORM(WINRT)

#define IS_READIUM_WRAPPED_XML(xml) (((xml) != nullptr) && ((xml)->_private != nullptr) && (*((unsigned int*)xml->_private) == _READIUM_XML_SIGNATURE))

// generic 'get me a wrapper' template
/**
 @ingroup xml-utils
 */
template <class _Tp, typename _Nm>
static inline std::shared_ptr<_Tp> Wrapped(_Nm * __n)
{
    typedef LibXML2Private<_Tp>*                _PrivatePtr;
    
    if ( __n == nullptr )
        return nullptr;
    
    // remember: _private is a *pointer to* a std::shared_ptr<_Tp> object
    try
    {
        // naive pagezero check
        if (__n->_private != nullptr && __n->_private > (void*)0x1000)
        {
            _PrivatePtr __p = reinterpret_cast<_PrivatePtr>(__n->_private);
            if (__p->__sig == _READIUM_XML_SIGNATURE)
            {
                return __p->__ptr;
            }
            else
            {
                throw std::logic_error("XML _private already carries a value!");
            }
        }
    }
    catch (...)
    {
        if (__n->_private != nullptr)
            throw std::logic_error("XML _private already carries a value!");
    }
    
    
    _PrivatePtr __p = new LibXML2Private<_Tp>(new _Tp(__n));
    __n->_private = __p;
    return __p->__ptr;
}

/**
 @ingroup xml-utils
 */
template <class _Tp, typename _Nm>
static inline void Rewrap(_Nm* __n, std::shared_ptr<_Tp> __t)
{
    typedef LibXML2Private<_Tp> _Private;
    
    if (__n == nullptr)
        return;
    
    try
    {
        if (IS_READIUM_WRAPPED_XML(__n))
        {
            _Private* __p = reinterpret_cast<_Private*>(__n->_private);
            if (__p->__ptr == __t)
                return;
            
            delete __p;
            __n->_private = nullptr;
        }
        
        __n->_private = new _Private(__t);
    }
    catch (...)
    {
    }
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

#if EPUB_PLATFORM(WINRT)
static inline ::Platform::String^ GetAttributeValueRecursiveNS(::Windows::Data::Xml::Dom::IXmlNode^ node, ::Platform::String^ uri, ::Platform::String^ name)
{
	::Windows::Data::Xml::Dom::IXmlElement^ element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(node);
	if (element == nullptr)
		element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(node->ParentNode);

	while (element != nullptr)
	{
		auto attr = element->GetAttributeNS(uri, name);
		if (attr != nullptr)
			return attr;
		element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(element->ParentNode);
	}

	return nullptr;
}
static inline ::Platform::String^ GetAttributeValueRecursive(::Windows::Data::Xml::Dom::IXmlNode^ node, ::Platform::String^ name)
{
	::Windows::Data::Xml::Dom::IXmlElement^ element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(node);
	if (element == nullptr)
		element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(node->ParentNode);

	while (element != nullptr)
	{
		auto attr = element->GetAttribute(name);
		if (attr != nullptr)
			return attr;
		element = dynamic_cast<::Windows::Data::Xml::Dom::IXmlElement^>(element->ParentNode);
	}

	return nullptr;
}
#endif

EPUB3_XML_END_NAMESPACE

#endif /* ePub3_xml_base_h */
