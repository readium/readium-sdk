//
//  archive_xml.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#include "archive_xml.h"
#include <ePub3/xml/document.h>
#include <ePub3/utilities/error_handler.h>
#include <sstream>

EPUB3_BEGIN_NAMESPACE

const int ArchiveXmlReader::DEFAULT_OPTIONS = XML_PARSE_RECOVER | XML_PARSE_DTDATTR | XML_PARSE_NONET;

#if ENABLE_XML_READ_DOC_MEMORY

std::shared_ptr<ePub3::xml::Document> ArchiveXmlReader::readXml(const ePub3::string& path)
{
    //size_t pos = _reader->position();

    size_t len = _reader->total_size();
    if (len <= 0)
        return nullptr;

    uint8_t *docBuf = (uint8_t*)malloc(len);
    if (!bool(docBuf))
        return nullptr;

    ssize_t resbuflen = _reader->read(docBuf, len);
    if (resbuflen <= 0)
    {
        free(docBuf);
        return nullptr;
    }

    // In some EPUBs, UTF-8 XML/HTML files have a superfluous (erroneous?) BOM, so we either:
    // pass "utf-8" and expect InputBuffer::read_cb (in io.cpp) to skip the 3 erroneous bytes
    // (otherwise the XML parser fails),
    // or we pass NULL (in which case the parser auto-detects encoding)
    const char * encoding = nullptr;
    //const char * encoding = "utf-8";

//    std::string fileContents ((char*)docBuf, resbuflen);

    xmlDocPtr raw = xmlReadMemory((const char*)docBuf, resbuflen, path.c_str(), encoding, ArchiveXmlReader::DEFAULT_OPTIONS);

    free(docBuf);

    if (!bool(raw) || (raw->type != XML_HTML_DOCUMENT_NODE && raw->type != XML_DOCUMENT_NODE) || !bool(raw->children)) {
        if (bool(raw)) {
            xmlFreeDoc(raw);
        }
        return nullptr;
    }

    std::shared_ptr<ePub3::xml::Document> doc = xml::Wrapped<ePub3::xml::Document>(raw);
    return doc;
}

#else

std::shared_ptr<xml::Document> ArchiveXmlReader::xmlReadDocument(const char * url, const char * encoding)
{
    return xml::InputBuffer::xmlReadDocument(url, encoding, ArchiveXmlReader::DEFAULT_OPTIONS);
}
//
//std::shared_ptr<xml::Document> ArchiveXmlReader::htmlReadDocument(const char * url, const char * encoding)
//{
//	return xml::InputBuffer::htmlReadDocument(url, encoding, ArchiveXmlReader::DEFAULT_OPTIONS);
//}

#endif //ENABLE_XML_READ_DOC_MEMORY

ArchiveXmlReader::ArchiveXmlReader(ArchiveReader * r) : _reader(r)
{
    if ( _reader == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Nil ArchiveReader supplied");
}
ArchiveXmlReader::ArchiveXmlReader(unique_ptr<ArchiveReader>&& r) : _reader(std::move(r))
{
    if ( _reader == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Nil ArchiveReader supplied");
}
ArchiveXmlReader::ArchiveXmlReader(ArchiveXmlReader&& o) : _reader(std::move(o._reader))
{
}
ArchiveXmlReader::~ArchiveXmlReader()
{
}
size_t ArchiveXmlReader::read(uint8_t *buf, size_t len)
{
    ssize_t r = _reader->read(buf, len);
    if ( r < 0 )
    {
        std::stringstream s;
        s << __PRETTY_FUNCTION__ << ": ArchiveReader::Read() returned " << r;
        HandleError(std::errc::io_error, s.str());
    }
    
    return static_cast<size_t>(r);
}
bool ArchiveXmlReader::close()
{
    return true;
}

#if ENABLE_ZIP_ARCHIVE_WRITER

ArchiveXmlWriter::ArchiveXmlWriter(ArchiveWriter* w) : _writer(w)
{
    if ( _writer == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Nil ArchiveWriter supplied");
}
ArchiveXmlWriter::ArchiveXmlWriter(ArchiveXmlWriter&& o) : _writer(std::move(o._writer))
{
}
ArchiveXmlWriter::~ArchiveXmlWriter()
{
}
bool ArchiveXmlWriter::write(const uint8_t *p, size_t len)
{
    size_t total = 0;
    ssize_t current = 0;
    while (total < len && current >= 0 )
    {
        current = _writer->write(p, len);
        if ( current > 0 )
            total += current;
    }
    
    return (total == len);
}
bool ArchiveXmlWriter::close()
{
    return true;
}

#endif //ENABLE_ZIP_ARCHIVE_WRITER

EPUB3_END_NAMESPACE

