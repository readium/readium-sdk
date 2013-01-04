//
//  archive_xml.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#ifndef __ePub3__archive_xml__
#define __ePub3__archive_xml__

#include "epub3.h"
#include "archive.h"
#include "../xml/utilities/io.h"

EPUB3_BEGIN_NAMESPACE

class ArchiveXmlReader : public xml::InputBuffer
{
public:
    ArchiveXmlReader(ArchiveReader * r);
    ArchiveXmlReader(const ArchiveXmlReader&) = delete;
    ArchiveXmlReader(ArchiveXmlReader&& o);
    virtual ~ArchiveXmlReader();
    
    operator ArchiveReader* () { return _reader; }
    operator const ArchiveReader* () const { return _reader; }
    
protected:
    ArchiveReader *     _reader;
    
    virtual size_t read(uint8_t * buf, size_t len);
    virtual bool close();
};

class ArchiveXmlWriter : public xml::OutputBuffer
{
public:
    ArchiveXmlWriter(ArchiveWriter * r);
    ArchiveXmlWriter(const ArchiveXmlWriter&&) = delete;
    ArchiveXmlWriter(ArchiveXmlWriter&& o);
    virtual ~ArchiveXmlWriter();
    
    operator ArchiveWriter* () { return _writer; }
    operator const ArchiveWriter* () { return _writer; }
    
protected:
    ArchiveWriter *     _writer;
    
    virtual bool write(const uint8_t *p, size_t len);
    virtual bool close();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__archive_xml__) */
