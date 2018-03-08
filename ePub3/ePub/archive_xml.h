//
//  archive_xml.h
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

#ifndef __ePub3__archive_xml__
#define __ePub3__archive_xml__

#include <ePub3/epub3.h>

#include <ePub3/archive.h>
#include <ePub3/xml/io.h>

EPUB3_BEGIN_NAMESPACE

/**
 @ingroup archives
 */
class ArchiveXmlReader
#if !ENABLE_XML_READ_DOC_MEMORY
		: public xml::InputBuffer
#endif //!ENABLE_XML_READ_DOC_MEMORY
{
public:
	/**
	 * The default options when reading XML documents from EPUB 3
	 * archives. To use with xmlReadDocument().
	 *
	 *   XML_PARSE_RECOVER: Recover on errors
	 *   XML_PARSE_NOENT:   Substitute entities
	 *   XML_PARSE_DTDATTR: Default DTD attributes
	 *   XML_PARSE_NONET:   Forbid network access (ie. when loading DTD)
	 */
    static const int DEFAULT_OPTIONS;
    
    EPUB3_EXPORT ArchiveXmlReader(ArchiveReader * r);
    EPUB3_EXPORT ArchiveXmlReader(unique_ptr<ArchiveReader>&& r);
    EPUB3_EXPORT ArchiveXmlReader(ArchiveXmlReader&& o);
    virtual ~ArchiveXmlReader();

	operator ArchiveReader* () { return _reader.get(); }
	operator const ArchiveReader* () const { return _reader.get(); }

	virtual size_t size() const { return _reader->total_size(); }
	virtual size_t offset() const { return _reader->position(); }
    
    bool operator !() const { return !bool(_reader); }

#if ENABLE_XML_READ_DOC_MEMORY

//#error "ENABLE_XML_READ_DOC_MEMORY = 1"
std::shared_ptr<ePub3::xml::Document> readXml(const ePub3::string& path);

#else

//	using InputBuffer::xmlReadDocument;
//	using InputBuffer::htmlReadDocument;

	/**
	 * Will read the given XML documents using the default options
	 * from ArchiveXmlReader::DEFAULT_OPTIONS.
	 */
	std::shared_ptr<xml::Document> xmlReadDocument(const char * url, const char * encoding);
//    std::shared_ptr<xml::Document> htmlReadDocument(const char * url, const char * encoding);

#endif //ENABLE_XML_READ_DOC_MEMORY

protected:
    std::unique_ptr<ArchiveReader>  _reader;
    
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
    
    ArchiveXmlReader(const ArchiveXmlReader&) _DELETED_;
};

#if ENABLE_ZIP_ARCHIVE_WRITER

/**
 @ingroup archives
 */
class ArchiveXmlWriter
#if !ENABLE_XML_READ_DOC_MEMORY
		: public xml::OutputBuffer
#endif //!ENABLE_XML_READ_DOC_MEMORY
{
public:
    EPUB3_EXPORT ArchiveXmlWriter(ArchiveWriter * r);
    EPUB3_EXPORT ArchiveXmlWriter(unique_ptr<ArchiveWriter>&& r);
    EPUB3_EXPORT ArchiveXmlWriter(ArchiveXmlWriter&& o);
    virtual ~ArchiveXmlWriter();
    
    operator ArchiveWriter* () { return _writer.get(); }
	operator const ArchiveWriter* () { return _writer.get(); }

	virtual size_t size() const { return _writer->total_size(); }
	virtual size_t offset() const { return _writer->position(); }
    
protected:
    unique_ptr<ArchiveWriter>   _writer;
    
    virtual bool write(const uint8_t *p, size_t len);
    virtual bool close();
    
    ArchiveXmlWriter(const ArchiveXmlWriter&&) _DELETED_;
};

#endif //ENABLE_ZIP_ARCHIVE_WRITER

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__archive_xml__) */
