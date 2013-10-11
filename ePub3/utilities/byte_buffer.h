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
     Removes a number of bytes from the beginning of the buffer.
     @param numBytesToRemove The number of bytes to remove.
     @note If this is used a lot, would be useful to maintain a start/end range rather
     than just a size-- this would make 'removal' much simpler and O(1).
     */
    void RemoveBytes(size_t numBytesToRemove);
    
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

	friend class ::Readium::BridgedByteBuffer;
};

EPUB3_END_NAMESPACE

#endif
