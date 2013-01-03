//
//  io.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-16.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3_xml_io__
#define __ePub3_xml_io__

#include "base.h"
#include <iostream>
#include <libxml/xmlIO.h>
#include <libxml/HTMLtree.h>

EPUB3_XML_BEGIN_NAMESPACE

class InputBuffer : public WrapperBase
{
public:
    InputBuffer();
    InputBuffer(InputBuffer && o) : _buf(o._buf) { o._buf = nullptr; }
    virtual ~InputBuffer();
    
    xmlParserInputBuffer * xmlBuffer() { return _buf; }
    const xmlParserInputBuffer * xmlBuffer() const { return _buf; }
    operator xmlParserInputBuffer * () { return xmlBuffer(); }
    operator const xmlParserInputBuffer * () const { return xmlBuffer(); }
    
    xmlDocPtr xmlReadDocument(const char * url, const char * encoding, int options);
    xmlDocPtr htmlReadDocument(const char * url, const char * encoding, int options);
    
protected:
    xmlParserInputBufferPtr _buf;
    
    virtual size_t read(uint8_t * buf, size_t len) = 0;
    virtual bool close() { return false; }
    
    static int read_cb(void * context, char * buffer, int len);
    static int close_cb(void * context);
    
};

class OutputBuffer : public WrapperBase
{
public:
    OutputBuffer(const std::string & encoding = std::string());
    OutputBuffer(OutputBuffer && o) : _buf(o._buf) { o._buf = nullptr; }
    virtual ~OutputBuffer();
    
    xmlOutputBuffer * xmlBuffer() { return _buf; }
    const xmlOutputBuffer * xmlBuffer() const { return _buf; }
    operator xmlOutputBuffer * () { return xmlBuffer(); }
    operator const xmlOutputBuffer * () const { return xmlBuffer(); }
    
    int writeDocument(xmlDocPtr doc);
    
protected:
    xmlOutputBufferPtr  _buf;
    
    virtual bool write(const uint8_t * buf, size_t len) = 0;
    virtual bool close() = 0;
    
    static int write_cb(void * context, const char * buffer, int len);
    static int close_cb(void * context);
    
};

class StreamInputBuffer : public InputBuffer
{
public:
    StreamInputBuffer(std::istream & input) : InputBuffer(), _input(input) {}
    StreamInputBuffer(StreamInputBuffer && o) : InputBuffer(std::move(o)), _input(o._input) {}
    virtual ~StreamInputBuffer() {}
    
protected:
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
    
    std::istream &  _input;
    
};

class StreamOutputBuffer : public OutputBuffer
{
public:
    StreamOutputBuffer(std::ostream & output, const std::string & encoding = std::string()) : OutputBuffer(), _output(output) {}
    StreamOutputBuffer(StreamOutputBuffer && o) : OutputBuffer(std::move(o)), _output(o._output) {}
    virtual ~StreamOutputBuffer() {}
    
protected:
    virtual bool write(const uint8_t * buffer, size_t len);
    virtual bool close();
    
    std::ostream &  _output;
    
};

EPUB3_XML_END_NAMESPACE

#endif /* defined(__ePub3_xml_io__) */
