//
//  byte_buffer.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "byte_buffer.h"
#include <stdexcept>
#import "CPUCacheUtils.h"

#if EPUB_OS(BSD)
# include <malloc/malloc.h>
# define GoodSize(size)     malloc_good_size(size)
#else
# define GoodSize(size)     size
#endif

#if EPUB_PLATFORM(WIN)
# define bzero(mem, size)   ZeroMemory(mem, size)
#endif

EPUB3_BEGIN_NAMESPACE

ByteBuffer::ByteBuffer(size_t bufferSize) : m_buffer(nullptr), m_bufferSize(0), m_bufferCapacity(0)
{
    size_t cap = GoodSize(bufferSize);
    m_buffer = reinterpret_cast<unsigned char*>(malloc(cap));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    
    bzero(m_buffer, bufferSize);
    m_bufferSize = bufferSize;
    m_bufferCapacity = cap;
}
ByteBuffer::ByteBuffer(const unsigned char* buffer, size_t bufferSize)
{
    size_t cap = GoodSize(bufferSize);
    m_buffer = reinterpret_cast<unsigned char*>(malloc(cap));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    
    memcpy(m_buffer, buffer, bufferSize);
    m_bufferSize = bufferSize;
    m_bufferCapacity = cap;
}
#if !EPUB_COMPILER_SUPPORTS(CXX_DELEGATING_CONSTRUCTORS)
ByteBuffer::ByteBuffer(const ByteBuffer& o)
{
    m_buffer = reinterpret_cast<unsigned char*>(malloc(o.m_bufferCapacity));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    
    memcpy(m_buffer, o.m_buffer, o.m_bufferSize);
    m_bufferSize = o.m_bufferSize;
    m_bufferCapacity = o.m_bufferCapacity;
}
#endif
ByteBuffer::~ByteBuffer()
{
    
    if ( m_buffer != nullptr )
    {
        if ( m_secure )
            Clean(m_buffer, m_bufferCapacity);
        free(m_buffer);
    }
    
    m_buffer = nullptr;
    m_bufferSize = 0;
    m_bufferCapacity = 0;
}

size_t ByteBuffer::MoveTo(unsigned char *targetBuffer, size_t targetBufferSize)
{
    if (m_bufferSize == 0)
    {
        return 0;
    }
    
    size_t resultLen = 0;
    
    if (m_bufferSize <= targetBufferSize)
    {
        ::memmove(targetBuffer, m_buffer, m_bufferSize);
        ::bzero(targetBuffer+m_bufferSize, targetBufferSize-m_bufferSize);
        resultLen = m_bufferSize;
        
        if ( m_secure )
            Clean(m_buffer, m_bufferCapacity);
        m_bufferSize = 0;       // allocation & capacity remain until Compact() is called
    }
    else
    {
        // move some bytes out
        ::memmove(targetBuffer, m_buffer, targetBufferSize);
        
        // determine the remainder
        unsigned char *remainderBuffer = m_buffer+targetBufferSize;
        size_t remainderLen = m_bufferSize-targetBufferSize;
        
        // move the remainder down
        ::memmove(m_buffer, remainderBuffer, remainderLen);
        
        // set the new length
        m_bufferSize = remainderLen;
        
        if ( m_secure )
            Clean(m_buffer+m_bufferSize, m_bufferCapacity-m_bufferSize);
        
        resultLen = targetBufferSize;
        // capacity remains until Compact() is called
    }
    
    return resultLen;
}

void ByteBuffer::AddBytes(unsigned char *extraBytes, size_t extraBytesSize)
{
    EnsureCapacity(m_bufferSize + extraBytesSize);
    memcpy(m_buffer+m_bufferSize, extraBytes, extraBytesSize);
    m_bufferSize += extraBytesSize;
}

void ByteBuffer::RemoveBytes(size_t numBytesToRemove)
{
    if (numBytesToRemove >= m_bufferSize)
    {
        m_bufferSize = 0;
        return;
    }
    
    size_t newBufferSize = m_bufferSize - numBytesToRemove;
    ::memmove(m_buffer, m_buffer+numBytesToRemove, newBufferSize);
    m_bufferSize = newBufferSize;
    
    if ( m_secure )
        Clean(m_buffer+m_bufferSize, m_bufferCapacity-m_bufferSize);
}

void ByteBuffer::Compact()
{
    if ( m_bufferCapacity > m_bufferSize )
    {
        if ( m_secure )
            Clean(m_buffer+m_bufferSize, m_bufferCapacity-m_bufferSize);
        
        m_buffer = reinterpret_cast<unsigned char*>(realloc(m_buffer, m_bufferSize));
        if ( m_buffer == nullptr )
            throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
        m_bufferCapacity = m_bufferSize;
    }
}

void ByteBuffer::EnsureCapacity(size_t desired)
{
    if ( m_bufferCapacity >= desired )
        return;
    
    size_t newCap = GoodSize(desired);
    m_buffer = reinterpret_cast<unsigned char*>(realloc(m_buffer, newCap));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    m_bufferCapacity = newCap;
    
    // zero trailing data
    if ( m_secure )
        Clean(m_buffer+m_bufferSize, m_bufferCapacity-m_bufferSize);
}

void ByteBuffer::Clean(unsigned char *ptr, size_t len)
{
    ::bzero(ptr, len);
    epub_sys_cache_flush(ptr, len);
}

EPUB3_END_NAMESPACE
