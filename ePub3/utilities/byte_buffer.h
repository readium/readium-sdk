//
//  byte_buffer.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_byte_buffer_h
#define ePub3_byte_buffer_h

#include <ePub3/epub3.h>

EPUB3_BEGIN_NAMESPACE

class ByteBuffer
{
public:
    
    ByteBuffer() : m_buffer(), m_bufferSize(0) {}
    ByteBuffer(size_t bufferSize) : m_buffer(new unsigned char[bufferSize]), m_bufferSize(bufferSize) {}
    ByteBuffer(unsigned char *buffer, size_t bufferSize) : m_buffer(buffer), m_bufferSize(bufferSize) {}
    ByteBuffer(ByteBuffer &&o) : m_buffer(std::move(o.m_buffer)), m_bufferSize(o.m_bufferSize) { o.m_bufferSize = 0; }
    virtual ~ByteBuffer() {}
    
    size_t MoveTo(unsigned char *targetBuffer, size_t targetBufferSize);
    bool IsEmpty() const { return (m_bufferSize == 0); }
    void AddBytes(unsigned char *extraBytes, size_t extraBytesSize);
    void ClearBytes(size_t numBytesToClear);
    unsigned char *GetBytes() { return m_buffer.get(); }
    size_t GetBufferSize() const { return m_bufferSize; }
    
private:
    
    ByteBuffer(const ByteBuffer &o) _DELETED_;
    
    unique_ptr<unsigned char[]> m_buffer;
    size_t m_bufferSize;
};

EPUB3_END_NAMESPACE

#endif
