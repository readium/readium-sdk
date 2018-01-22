//
//  ring_buffer.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-06.
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

#include "ring_buffer.h"
#include <cstring>
#include <cstdlib>

EPUB3_BEGIN_NAMESPACE
RingBuffer::RingBuffer(std::size_t size) : _capacity(size), _numBytes(0), _readPos(0), _writePos(0), _lock()
{
    _buffer = new uint8_t[_capacity];
}
RingBuffer::RingBuffer(const RingBuffer& o) : _capacity(o._capacity), _numBytes(0), _readPos(0), _writePos(0), _lock()
{
    _buffer = new uint8_t[_capacity];
    
    std::lock_guard<RingBuffer> _(const_cast<RingBuffer&>(o));
    
    _numBytes = o._numBytes;
    _readPos = o._readPos;
    _writePos = o._writePos;
    
    std::memcpy(_buffer, o._buffer, _capacity);
}
RingBuffer::RingBuffer(RingBuffer&& o) : _capacity(o._capacity), _numBytes(0), _readPos(0), _writePos(0), _lock()
{
    std::lock_guard<RingBuffer> _(o);
    
    _buffer = o._buffer;            o._buffer = nullptr;
    _numBytes = o._numBytes;        o._numBytes = 0;
    _readPos = o._readPos;          o._readPos = 0;
    _writePos = o._writePos;        o._writePos = 0;
}
RingBuffer::~RingBuffer()
{
    // may have been consumed by a move constructor or assignment operator
    if ( _buffer != nullptr )
        delete [] _buffer;
}
RingBuffer& RingBuffer::operator=(const RingBuffer& o)
{
    if ( o._capacity > _capacity )
    {
        if ( _buffer != nullptr )
            delete [] _buffer;
        _buffer = new uint8_t[o._capacity];
        _capacity = o._capacity;
    }
    
    std::lock_guard<RingBuffer>(const_cast<RingBuffer&>(o));
    
    _numBytes = o._numBytes;
    _readPos = o._readPos;
    _writePos = o._writePos;
    
    std::memcpy(_buffer, o._buffer, _capacity);
    return *this;
}
RingBuffer& RingBuffer::operator=(RingBuffer&& o)
{
    std::lock_guard<RingBuffer> _(o);
    
    if ( _buffer != nullptr )
        delete [] _buffer;
    
    _buffer = o._buffer;            o._buffer = 0;
    _numBytes = o._numBytes;        o._numBytes = 0;
    _readPos = o._readPos;          o._readPos = 0;
    _writePos = o._writePos;        o._writePos = 0;
    return *this;
}
std::size_t RingBuffer::ReadBytes(uint8_t *buf, std::size_t len)
{
    std::size_t copied = std::min(len, _numBytes);
    if ( copied != 0 )
    {
        if ( _readPos < _writePos )
        {
            std::memcpy(buf, &_buffer[_readPos], len);
        }
        else
        {
            std::size_t __t = _capacity - _readPos;
            std::memcpy(buf, &_buffer[_readPos], __t);
            std::memcpy(&buf[__t], _buffer, len - __t);
        }
    }
    
    return copied;
}
std::size_t RingBuffer::WriteBytes(const uint8_t *buf, std::size_t len)
{
    std::size_t copied = std::min(len, SpaceAvailable());
    if ( copied != 0 )
    {
        if ( _writePos < _readPos )
        {
            std::memcpy(&_buffer[_writePos], buf, len);
            _writePos += len;
        }
        else
        {
            std::size_t __t = _capacity - _writePos;
            if ( __t >= len )
            {
                std::memcpy(&_buffer[_writePos], buf, len);
                _writePos += len;
            }
            else
            {
                std::size_t __b = len - __t;
                std::memcpy(&_buffer[_writePos], buf, __t);
                std::memcpy(_buffer, &buf[__t], __b);
                _writePos = __b;
            }
        }
        
        if ( _writePos == _capacity )
            _writePos = 0;
        _numBytes += len;
    }
    
    return copied;
}
void RingBuffer::RemoveBytes(std::size_t len) _NOEXCEPT
{
    _readPos += len;
    if ( _readPos >= _capacity )
        _readPos -= _capacity;
    _numBytes -= len;
}

EPUB3_END_NAMESPACE
