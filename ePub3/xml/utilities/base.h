//
//  base.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-16.
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
        : __sig(_READIUM_XML_SIGNATURE), __ptr(
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        std::shared_ptr<_Tp>(__p)
#else
        __p
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        )
        {}
    LibXML2Private(std::shared_ptr<_Tp>& __p)
        : __sig(_READIUM_XML_SIGNATURE), __ptr(__p)
        {}
    ~LibXML2Private()
        { __sig = 0xbaadf00d; }
    
    // data member-- used to determine if this is a Readium-made pointer
    unsigned int __sig;
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
        std::weak_ptr<_Tp> __ptr;
#else
        std::shared_ptr<_Tp> __ptr;
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER

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
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
				std::shared_ptr<_Tp> toRet = __p->__ptr.lock();
				if (!bool(toRet))// if there is no live reference to the readium wrapper, but
				{
					delete __p;	// release unreferenced memory, if any
					__n->_private = nullptr;

					std::shared_ptr<_Tp> toRet = std::shared_ptr<_Tp>(new _Tp(__n));

					//_PrivatePtr __p = new LibXML2Private<_Tp>(new _Tp(__n));
					_PrivatePtr __p = new LibXML2Private<_Tp>(toRet);
					__n->_private = __p;
					//return __p->__ptr;
					return toRet;
				}

				return toRet;
#else
                return __p->__ptr;
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
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

#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    std::shared_ptr<_Tp> toRet = std::shared_ptr<_Tp>(new _Tp(__n));
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER

    _PrivatePtr __p = new LibXML2Private<_Tp>(
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    toRet
#else
    new _Tp(__n)
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    );
    __n->_private = __p;
    return
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
    toRet;
#else
    __p->__ptr;
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
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
            if (
#if ENABLE_WEAK_PTR_XML_NODE_WRAPPER
            __p->__ptr.lock() && __p->__ptr.lock() == __t
#else
            __p->__ptr == __t
#endif //ENABLE_WEAK_PTR_XML_NODE_WRAPPER
            )
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
