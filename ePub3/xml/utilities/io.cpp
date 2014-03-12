//
//  io.cpp
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

#include "io.h"
#include "../tree/document.h"

EPUB3_XML_BEGIN_NAMESPACE

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
    return static_cast<int>(p->read(reinterpret_cast<uint8_t*>(buffer), static_cast<size_t>(len)));
}
int InputBuffer::close_cb(void *context)
{
    InputBuffer * p = static_cast<InputBuffer*>(context);
    return (p->close() ? 0 : -1);
}
std::shared_ptr<Document> InputBuffer::xmlReadDocument(const char * url, const char * encoding, int options)
{
    xmlDocPtr raw = xmlReadIO(_buf->readcallback, _buf->closecallback, _buf->context, url, encoding, options);
    return Wrapped<Document>(raw);
}
std::shared_ptr<Document> InputBuffer::htmlReadDocument(const char *url, const char *encoding, int options)
{
    return Wrapped<Document>(htmlReadIO(_buf->readcallback, _buf->closecallback, _buf->context, url, encoding, options));
}

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
    size_t result = _input.seekg(0, std::ios::end).tellg();
    _input.seekg(pos);
    return result;
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
    size_t result = _output.seekp(0, std::ios::end).tellp();
    _output.seekp(pos);
    return result;
}

EPUB3_XML_END_NAMESPACE
