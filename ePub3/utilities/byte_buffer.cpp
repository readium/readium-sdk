//
//  byte_buffer.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#define _EPUB3_BUILDING_BYTE_BUFFER

#include "byte_buffer.h"
#include <stdexcept>
#include <system_error>
#include "CPUCacheUtils.h"

#if EPUB_OS(BSD)
# include <malloc/malloc.h>
# define GoodSize(size)     malloc_good_size(size)
#else
# define GoodSize(size)     size + 1
#endif

#if EPUB_OS(WINDOWS)
#include <Windows.h>
# define bzero(mem, size)   ZeroMemory(mem, size)
#endif

EPUB3_BEGIN_NAMESPACE

const prealloc_buf_t prealloc_buf = {};

ByteBuffer::ByteBuffer(size_t bufferSize) : m_buffer(nullptr), m_bufferSize(0), m_bufferCapacity(0)
{
    size_t cap = GoodSize(bufferSize);
	m_buffer = reinterpret_cast<unsigned char*>(calloc(cap, sizeof(unsigned char)));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    
    m_bufferSize = bufferSize;
    m_bufferCapacity = cap;
}
ByteBuffer::ByteBuffer(size_t bufferSize, prealloc_buf_t) : m_buffer(nullptr), m_bufferSize(0), m_bufferCapacity(0)
{
    size_t cap = GoodSize(bufferSize);
	m_buffer = reinterpret_cast<unsigned char*>(calloc(cap, sizeof(unsigned char)));
    if ( m_buffer == nullptr )
        throw std::system_error(std::make_error_code(std::errc::not_enough_memory), "ByteBuffer");
    
    m_bufferCapacity = cap;
}
ByteBuffer::ByteBuffer(const unsigned char* buffer, size_t bufferSize)
{
    size_t cap = GoodSize(bufferSize);
	m_buffer = reinterpret_cast<unsigned char*>(calloc(cap, sizeof(unsigned char)));
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

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& o)
{
    EnsureCapacity(o.m_bufferCapacity);
    if ( m_secure && o.m_bufferSize < m_bufferSize )
        Clean(m_buffer+o.m_bufferSize, m_bufferSize-o.m_bufferSize);
    ::memcpy(m_buffer, o.m_buffer, o.m_bufferSize);
    
    m_bufferSize = o.m_bufferSize;
    return *this;
}
ByteBuffer& ByteBuffer::operator=(ByteBuffer&& o)
{
    if ( m_buffer != nullptr )
    {
        if ( m_secure )
            Clean(m_buffer, m_bufferCapacity);
        ::free(m_buffer);
    }
    
    m_buffer = o.m_buffer;
    m_bufferSize = o.m_bufferSize;
    m_bufferCapacity = o.m_bufferCapacity;
    m_secure = o.m_secure;
    
    o.m_buffer = nullptr;
    o.m_bufferSize = o.m_bufferCapacity = 0;
    o.m_secure = false;
    
    return *this;
}

bool ByteBuffer::operator==(const ByteBuffer& o) const
{
    if ( m_bufferSize != o.m_bufferSize )
        return false;
    return (::memcmp(m_buffer, o.m_buffer, m_bufferSize) == 0);
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

void ByteBuffer::RemoveBytes(size_t numBytesToRemove, size_t pos)
{
	if (numBytesToRemove >= m_bufferSize && pos == 0)
    {
        m_bufferSize = 0;
        return;
    }

	numBytesToRemove = std::min(numBytesToRemove, m_bufferSize - pos);
    
    size_t newBufferSize = m_bufferSize - numBytesToRemove;
	if (pos < m_bufferSize - numBytesToRemove)
	{
		::memmove(m_buffer + pos, m_buffer + pos + numBytesToRemove, newBufferSize);
	}

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
