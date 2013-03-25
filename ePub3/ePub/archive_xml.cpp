//
//  archive_xml.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-29.
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

#include "archive_xml.h"
#include <sstream>

EPUB3_BEGIN_NAMESPACE

ArchiveXmlReader::ArchiveXmlReader(ArchiveReader * r) : _reader(r)
{
    if ( _reader == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Nil ArchiveReader supplied");
}
ArchiveXmlReader::ArchiveXmlReader(ArchiveXmlReader&& o) : _reader(o._reader)
{
    o._reader = nullptr;
}
ArchiveXmlReader::~ArchiveXmlReader()
{
    if ( _reader != nullptr )
        delete _reader;
}
size_t ArchiveXmlReader::read(uint8_t *buf, size_t len)
{
    ssize_t r = _reader->read(buf, len);
    if ( r < 0 )
    {
        std::stringstream s(std::string(__PRETTY_FUNCTION__) + ": ArchiveReader::Read() returned ");
        s << r;
        throw std::runtime_error(s.str());
    }
    
    return static_cast<size_t>(r);
}
bool ArchiveXmlReader::close()
{
    return true;
}

ArchiveXmlWriter::ArchiveXmlWriter(ArchiveWriter* w) : _writer(w)
{
    if ( _writer == nullptr )
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + ": Nil ArchiveWriter supplied");
}
ArchiveXmlWriter::ArchiveXmlWriter(ArchiveXmlWriter&& o) : _writer(o._writer)
{
    o._writer = nullptr;
}
ArchiveXmlWriter::~ArchiveXmlWriter()
{
    if ( _writer != nullptr )
        delete _writer;
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

EPUB3_END_NAMESPACE
