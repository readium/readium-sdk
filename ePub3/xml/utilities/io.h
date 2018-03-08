//
//  io.h
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

#ifndef __ePub3_xml_io__
#define __ePub3_xml_io__

#include <ePub3/xml/base.h>
#include <iostream>
#include <cstdint>
#if EPUB_USE(LIBXML2)
#include <libxml/xmlIO.h>
#include <libxml/HTMLtree.h>
#endif

EPUB3_XML_BEGIN_NAMESPACE

class Document;

#if EPUB_USE(WIN_XML)
enum {
	PROHIBIT_DTD = 1 << 0,
	RESOLVE_EXTERNALS = 1 << 1,
	VALIDATE_ON_PARSE = 1 << 2
};
typedef unsigned int XmlOptions;
#endif

#if !ENABLE_XML_READ_DOC_MEMORY

/**
 @ingroup xml-utils
 */
class InputBuffer : public WrapperBase<InputBuffer>
{
public:
    InputBuffer();
	InputBuffer(InputBuffer && o);
    virtual ~InputBuffer();
#if EPUB_USE(LIBXML2)
    xmlParserInputBuffer * xmlBuffer() { return _buf; }
    const xmlParserInputBuffer * xmlBuffer() const { return _buf; }
    operator xmlParserInputBuffer * () { return xmlBuffer(); }
    operator const xmlParserInputBuffer * () const { return xmlBuffer(); }

    std::shared_ptr<Document> xmlReadDocument(const char * url, const char * encoding, int options);
//    std::shared_ptr<Document> htmlReadDocument(const char * url, const char * encoding, int options);

#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^ File() { return _store; }
	operator ::Windows::Storage::IStorageFile^() { return _store; }

	std::shared_ptr<Document> ReadDocument(const char* url, const char* encoding, XmlOptions options);
#endif

	virtual size_t size() const = 0;
	virtual size_t offset() const = 0;
    
    virtual
    void release() OVERRIDE
        {}

protected:
            const char * _encodingCheck;

#if EPUB_USE(LIBXML2)
    xmlParserInputBufferPtr _buf;
#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^ _store;
#endif
    
    virtual size_t read(uint8_t * buf, size_t len) = 0;
    virtual bool close() { return false; }
    
    static int read_cb(void * context, char * buffer, int len);
    static int close_cb(void * context);
    
};


/**
 @ingroup xml-utils
 */
class StreamInputBuffer : public InputBuffer
{
public:
    StreamInputBuffer(std::istream & input) : InputBuffer(), _input(input) {}
    StreamInputBuffer(StreamInputBuffer && o) : InputBuffer(std::move(o)), _input(o._input) {}
    virtual ~StreamInputBuffer() {}
    
    virtual size_t size() const OVERRIDE;
    virtual size_t offset() const OVERRIDE { return size_t(_input.tellg()); }
    
protected:
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
    
    std::istream &  _input;
    
};


#endif //ENABLE_XML_READ_DOC_MEMORY


/**
 @ingroup xml-utils
 */
class OutputBuffer : public WrapperBase<OutputBuffer>
{
public:
	OutputBuffer(const std::string & encoding = std::string());
#if EPUB_USE(LIBXML2)
	OutputBuffer(OutputBuffer && o) : _buf(o._buf) { o._buf = nullptr; }
#elif EPUB_USE(WIN_XML)
	OutputBuffer(OutputBuffer && o) : _store(o._store) { o._store = nullptr; }
#endif
	virtual ~OutputBuffer();
#if EPUB_USE(LIBXML2)
	xmlOutputBuffer * xmlBuffer() { return _buf; }
    const xmlOutputBuffer * xmlBuffer() const { return _buf; }
    operator xmlOutputBuffer * () { return xmlBuffer(); }
    operator const xmlOutputBuffer * () const { return xmlBuffer(); }

    void flush() { xmlOutputBufferFlush(_buf); }

	int writeDocument(xmlDocPtr doc);
#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^ File() { return _store; }
	operator ::Windows::Storage::IStorageFile^() { return _store; }

	int WriteDocument(std::shared_ptr<const Document> doc);
#endif
	virtual size_t size() const = 0;
	virtual size_t offset() const = 0;

	virtual
	void release() OVERRIDE
	{}

protected:
#if EPUB_USE(LIBXML2)
	xmlOutputBufferPtr  _buf;
#elif EPUB_USE(WIN_XML)
	::Windows::Storage::IStorageFile^	_store;
#endif

	virtual bool write(const uint8_t * buf, size_t len) = 0;
	virtual bool close() { return false; }

	static int write_cb(void * context, const char * buffer, int len);
	static int close_cb(void * context);

};
/**
 @ingroup xml-utils
 */
class StreamOutputBuffer : public OutputBuffer
{
public:
    StreamOutputBuffer(std::ostream & output, const std::string & encoding = std::string()) : OutputBuffer(), _output(output) {}
    StreamOutputBuffer(StreamOutputBuffer && o) : OutputBuffer(std::move(o)), _output(o._output) {}
    virtual ~StreamOutputBuffer() {}
    
    virtual size_t size() const OVERRIDE;
    virtual size_t offset() const OVERRIDE { return size_t(_output.tellp()); }
    
protected:
    virtual bool write(const uint8_t * buffer, size_t len);
    virtual bool close();
    
    std::ostream &  _output;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_io__) */
