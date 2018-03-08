//
//  byte_buffer.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
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

#ifndef ePub3_byte_buffer_h
#define ePub3_byte_buffer_h

#include <ePub3/epub3.h>

#if EPUB_PLATFORM(WINRT)
namespace Readium { class BridgedByteBuffer; };
#endif

EPUB3_BEGIN_NAMESPACE

/**
 Special value passed to ByteBuffer initializer to tell it to only set its capacity,
 not its size.
 */
struct EPUB3_EXPORT prealloc_buf_t {};

#if !EPUB_COMPILER_SUPPORTS(CXX_CONSTEXPR) || defined(_EPUB3_BUILDING_BYTE_BUFFER)

extern const prealloc_buf_t prealloc_buf;

#else

constexpr prealloc_buf_t prealloc_buf = prealloc_buf_t();

#endif

class ByteBuffer
{
public:
    
    ByteBuffer() : m_buffer(nullptr), m_bufferSize(0), m_bufferCapacity(0) {}
    ByteBuffer(size_t bufferSize);
    ByteBuffer(size_t bufferSize, prealloc_buf_t);
    ByteBuffer(const unsigned char *buffer, size_t bufferSize);   // copy-in
#if EPUB_COMPILER_SUPPORTS(CXX_DELEGATING_CONSTRUCTORS)
    ByteBuffer(const ByteBuffer& o) : ByteBuffer(o.m_buffer, o.m_bufferSize) {}
#else
    ByteBuffer(const ByteBuffer& o);
#endif
    ByteBuffer(ByteBuffer &&o) : m_buffer(std::move(o.m_buffer)), m_bufferSize(o.m_bufferSize), m_bufferCapacity(o.m_bufferCapacity) { o.m_bufferSize = o.m_bufferCapacity = 0; }
    virtual ~ByteBuffer();
    
    ByteBuffer& operator=(const ByteBuffer&);
    ByteBuffer& operator=(ByteBuffer&&);
    
    bool operator==(const ByteBuffer&) const;
    bool operator!=(const ByteBuffer& o) const { return !(*this == o); }
    
    /**
     Tells the buffer to perform secure erasure by zeroing all unused memory.
     
     This will also trigger a data cache flush where supported.
     @param value `true` to perform secure erasure, `false` otherwise.
     */
    void SetUsesSecureErasure(bool value=true) { m_secure = value; }
    bool UsesSecureErasure() const { return m_secure; }
    
    /**
     Moves bytes from the receiver into another memory range.
     
     The receiver keeps its allocation and storage, though its size will be reduced.
     If data remains in the receiver, the remainder will be moved to the start of its
     internal storage.
     
     Call Compact() to collapse the size of the receiver's buffer.
     
     @param targetBuffer A pointer to memory into which to copy the receiver's content.
     @param targetBufferSize The size of targetBuffer.
     @result The number of bytes copied into targetBuffer.
     */
    size_t MoveTo(unsigned char *targetBuffer, size_t targetBufferSize);
    bool IsEmpty() const { return (m_bufferSize == 0); }
    
    /**
     Appends bytes to the internal buffer.
     @param extraBytes A pointer to the bytes to append.
     @param extraBytesSize The number of bytes in extraBytes.
     */
    void AddBytes(unsigned char *extraBytes, size_t extraBytesSize);
    
    /**
     Removes a number of bytes from the the buffer.
     @param numBytesToRemove The number of bytes to remove.
     @note If this is used a lot, would be useful to maintain a start/end range rather
     than just a size-- this would make 'removal' much simpler and O(1).
     */
    void RemoveBytes(size_t numBytesToRemove, size_t pos=0);
    
    unsigned char* GetBytes() { return m_buffer; }
    const unsigned char* GetBytes() const { return m_buffer; }
    size_t GetBufferSize() const { return m_bufferSize; }
    
    /**
     Ensures that the receiver takes up only the amount of memory that is actually
     required.
     */
    void Compact();
    
private:
    
    void EnsureCapacity(size_t desired);
    void Clean(unsigned char* ptr, size_t len);
    
    // the object is managing this memory, so a raw pointer is acceptable here
    unsigned char* m_buffer;
    // size of actual data
    size_t m_bufferSize;
    // actual allocated capacity (may be more)
    size_t m_bufferCapacity;
    // whether to zero unused bytes
    bool m_secure;
#if EPUB_PLATFORM(WINRT)
	friend class ::Readium::BridgedByteBuffer;
#endif
};

EPUB3_END_NAMESPACE

#endif
