//
//  io.cpp
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

#include "io.h"
#include "../tree/document.h"

EPUB3_XML_BEGIN_NAMESPACE

#if !ENABLE_XML_READ_DOC_MEMORY

InputBuffer::InputBuffer()
{
    _buf = xmlParserInputBufferCreateIO(InputBuffer::read_cb, InputBuffer::close_cb, this, XML_CHAR_ENCODING_NONE);
    if ( _buf == NULL )
        throw InternalError("Failed to create xml input buffer");
}
InputBuffer::~InputBuffer()
{
    xmlFreeParserInputBuffer(_buf);
    _buf = nullptr;
}

int InputBuffer::read_cb(void *context, char *buffer, int len)
{
    InputBuffer * p = reinterpret_cast<InputBuffer*>(context);

    size_t toRead = static_cast<size_t>(len);
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);

    size_t res = 0;
    if (p->_encodingCheck != NULL && (std::strcmp(p->_encodingCheck, "utf-8") == 0) && len >= 3)
    {
        res = p->read(buf, 3);

        // BOM check (0xEF,0xBB,0xBF)
        if (res == 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
        {
            // Skip BOM bytes
            res = p->read(buf, toRead - 3);
        }
        else if (res > 0)
        {
            // no BOM, read more bytes
            res += p->read(buf + res, toRead - res);
        }
    }
    else
    {
        res = p->read(buf, toRead);
    }
    p->_encodingCheck = NULL;

    return static_cast<int>(res);
}
int InputBuffer::close_cb(void *context)
{
    InputBuffer * p = static_cast<InputBuffer*>(context);
    return (p->close() ? 0 : -1);
}
std::shared_ptr<Document> InputBuffer::xmlReadDocument(const char * url, const char * encoding, int options)
{
    _encodingCheck = encoding;
    //xmlDocPtr raw = xmlReadIO(_buf->readcallback, _buf->closecallback, _buf->context, url, encoding, options);
    xmlDocPtr raw = xmlReadIO(_buf->readcallback, _buf->closecallback, _buf->context, url, encoding, options);
    if (!bool(raw) || (raw->type != XML_HTML_DOCUMENT_NODE && raw->type != XML_DOCUMENT_NODE) || !bool(raw->children)) {
        if (bool(raw)) {
            xmlFreeDoc(raw);
        }
        return nullptr;
    }
    return Wrapped<Document>(raw);
}
//std::shared_ptr<Document> InputBuffer::htmlReadDocument(const char *url, const char *encoding, int options)
//{
//    _encodingCheck = encoding;
//    // is in fact an xmlDocPtr
//    htmlDocPtr raw = htmlReadIO(_buf->readcallback, _buf->closecallback, _buf->context, url, encoding, options);
//    if (!bool(raw) || (raw->type != XML_HTML_DOCUMENT_NODE && raw->type != XML_DOCUMENT_NODE) || !bool(raw->children)) {
//        if (bool(raw)) {
//            xmlFreeDoc(raw);
//        }
//        return nullptr;
//    }
//    return Wrapped<Document>(raw);
//}


size_t StreamInputBuffer::read(uint8_t *buf, size_t len)
{
    size_t num = 0;
    if ( _input.good() )
        num = static_cast<size_t>(_input.readsome(reinterpret_cast<std::istream::char_type*>(buf), len));
    return num;
}
bool StreamInputBuffer::close()
{
    return true;
}
size_t StreamInputBuffer::size() const
{
    std::istream::pos_type pos = _input.tellg();
    size_t result = (size_t)_input.seekg(0, std::ios::end).tellg();
    _input.seekg(pos);
    return result;
}
#endif //!ENABLE_XML_READ_DOC_MEMORY

OutputBuffer::OutputBuffer(const std::string & encoding)
{
    xmlCharEncodingHandlerPtr encoder = nullptr;
    if ( !encoding.empty() )
    {
        xmlCharEncoding enc = xmlParseCharEncoding(encoding.c_str());
        if ( enc != XML_CHAR_ENCODING_UTF8 )
        {
            encoder = xmlFindCharEncodingHandler(encoding.c_str());
            if ( encoder == nullptr )
                throw InternalError("Unsupported output encoding: " + encoding);
        }
    }

    _buf = xmlOutputBufferCreateIO(OutputBuffer::write_cb, OutputBuffer::close_cb, this, encoder);
    if ( _buf == nullptr )
        throw InternalError("Failed to create xml output buffer");
}
OutputBuffer::~OutputBuffer()
{
    xmlMemFree(_buf);
    _buf = nullptr;
}
int OutputBuffer::write_cb(void *context, const char *buffer, int len)
{
    OutputBuffer * p = reinterpret_cast<OutputBuffer*>(context);
    return (p->write(reinterpret_cast<const uint8_t*>(buffer), static_cast<size_t>(len)) ? len : -1);
}
int OutputBuffer::close_cb(void *context)
{
    OutputBuffer * p = reinterpret_cast<OutputBuffer*>(context);
    return (p->close() ? 0 : -1);
}
int OutputBuffer::writeDocument(xmlDocPtr doc)
{
    return xmlSaveFileTo(*this, doc, "utf-8");
}

bool StreamOutputBuffer::write(const uint8_t *buffer, size_t len)
{
    // std::basic_ios::operator bool () is EXPLICIT in C++11/libstdc++
    if ( _output.good() )
        _output.write(reinterpret_cast<const std::ostream::char_type*>(buffer), len);
    return _output.good();
}
bool StreamOutputBuffer::close()
{
    _output.flush();
    return true;
}
size_t StreamOutputBuffer::size() const
{
    std::ostream::pos_type pos = _output.tellp();
    size_t result = (size_t)_output.seekp(0, std::ios::end).tellp();
    _output.seekp(pos);
    return result;
}

EPUB3_XML_END_NAMESPACE
