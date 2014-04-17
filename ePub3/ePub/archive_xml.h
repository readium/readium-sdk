//
//  archive_xml.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#ifndef __ePub3__archive_xml__
#define __ePub3__archive_xml__

#include <ePub3/epub3.h>

#include <ePub3/archive.h>
#include <ePub3/xml/io.h>

EPUB3_BEGIN_NAMESPACE

/**
 @ingroup archives
 */
class ArchiveXmlReader : public xml::InputBuffer
{
public:
    EPUB3_EXPORT ArchiveXmlReader(ArchiveReader * r);
    EPUB3_EXPORT ArchiveXmlReader(unique_ptr<ArchiveReader>&& r);
    EPUB3_EXPORT ArchiveXmlReader(ArchiveXmlReader&& o);
    virtual ~ArchiveXmlReader();
    
    operator ArchiveReader* () { return _reader.get(); }
	operator const ArchiveReader* () const { return _reader.get(); }

	virtual size_t size() const { return _reader->total_size(); }
	virtual size_t offset() const { return _reader->position(); }
    
    bool operator !() const { return !bool(_reader); }
    
protected:
    std::unique_ptr<ArchiveReader>  _reader;
    
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
    
    ArchiveXmlReader(const ArchiveXmlReader&) _DELETED_;
};

/**
 @ingroup archives
 */
class ArchiveXmlWriter : public xml::OutputBuffer
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

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__archive_xml__) */
