//
//  byte_buffer.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "byte_buffer.h"

EPUB3_BEGIN_NAMESPACE

size_t ByteBuffer::MoveTo(unsigned char *targetBuffer, size_t targetBufferSize)
{
    if (m_bufferSize == 0)
    {
        return 0;
    }
    
    size_t resultLen = 0;
    
    if (m_bufferSize <= targetBufferSize)
    {
        ::memset(targetBuffer, 0, targetBufferSize);
        ::memcpy(targetBuffer, m_buffer.get(), m_bufferSize);
        resultLen = m_bufferSize;
        m_buffer.reset();
        m_bufferSize = 0;
    }
    else
    {
        unsigned char *reminderBuffer = m_buffer.get();
        ::memcpy(targetBuffer, reminderBuffer, targetBufferSize);
        reminderBuffer += targetBufferSize;
        m_bufferSize -= targetBufferSize;
        unsigned char *newBuffer = new unsigned char[m_bufferSize];
        ::memcpy(newBuffer, reminderBuffer, m_bufferSize);
        m_buffer.reset(newBuffer);
        resultLen = targetBufferSize;
    }
    
    return resultLen;
}

void ByteBuffer::AddBytes(unsigned char *extraBytes, size_t extraBytesSize)
{
    if (m_bufferSize == 0)
    {
        unsigned char *newBuffer = new unsigned char[extraBytesSize];
        ::memcpy(newBuffer, extraBytes, extraBytesSize);
        m_bufferSize = extraBytesSize;
        m_buffer.reset(newBuffer);
        return;
    }
    
    unsigned char *newBuffer = new unsigned char[m_bufferSize + extraBytesSize];
    ::memcpy(newBuffer, m_buffer.get(), m_bufferSize);
    ::memcpy(newBuffer + m_bufferSize, extraBytes, extraBytesSize);
    m_bufferSize += extraBytesSize;
    m_buffer.reset(newBuffer);
}

void ByteBuffer::ClearBytes(size_t numBytesToClear)
{
    if (numBytesToClear >= m_bufferSize)
    {
        m_bufferSize = 0;
        m_buffer.reset();
        return;
    }
    
    size_t newBufferSize = m_bufferSize - numBytesToClear;
    unsigned char *newBuffer = new unsigned char[newBufferSize];
    ::memcpy(newBuffer, m_buffer.get() + numBytesToClear, newBufferSize);
    m_bufferSize = newBufferSize;
    m_buffer.reset(newBuffer);
}

EPUB3_END_NAMESPACE
