//
//  io.h
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
    std::shared_ptr<Document> htmlReadDocument(const char * url, const char * encoding, int options);
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
