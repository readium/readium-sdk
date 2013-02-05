//
//  ring_buffer.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
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

#ifndef __ePub3__ring_buffer__
#define __ePub3__ring_buffer__

#include "epub3.h"
#include "basic.h"
#include "rw_lock.h"
#include <array>
#include <mutex>

EPUB3_BEGIN_NAMESPACE

/**
 This class implements a ring buffer with a statically-allocated buffer.
 
 The function of a ring buffer is that when data is removed, only a read-position
 marker is updated; this means that any remaining data is not copied or moved.  Note
 also that this implementation uses a statically-allocated buffer of a constant size
 (the default is 4096 bytes), and that the buffer cannot grow.  This means that you
 must pay attention to the amount of space available in the ring buffer when reading
 data from any persistent storage to be placed herein: only read as much as you can
 store here.
 
 The ring buffer is used as a component in the ByteStream classes to enable
 asynchronous reading/writing behaviour.
 */
template <size_t _Bufsize=4096>
class RingBuffer
{
public:
    ///
    /// Constructs a new RingBuffer instance.
                    RingBuffer()                        noexcept : _numBytes(0), _readPos(0), _writePos(0), _lock() {}
    ///
    /// Destructor.
    virtual         ~RingBuffer()                       noexcept    {}
    
    ///
    /// RingBuffer instances cannot be copied (locking issues).
                    RingBuffer(const RingBuffer&)       = delete;
    ///
    /// RingBuffer instances cannot be moved (buffer would have to be copied anyway).
                    RingBuffer(RingBuffer&&)            = delete;
    
    /**
     @defgroup Metadata Buffer Metadata
     @{
     */
    
    /**
     Obtain the total capacity of a ring buffer.
     @result The maximum number of bytes the buffer can hold.
     */
    std::size_t     Capacity()              const noexcept  { return _Bufsize; }
    
    /**
     @return `true` is there is data in the buffer, `false` otherwise.
     */
    bool            HasData()               const noexcept  { return _numBytes != 0; }
    
    /**
     @return The number of bytes available to read from the buffer.
     */
    std::size_t     BytesAvailable()        const noexcept  { return _numBytes; }
    
    /**
     @return `true` if there is room to write data to the buffer.
     */
    bool            HasSpace()              const noexcept  { return _numBytes != _Bufsize; }
    
    /**
     @return The maximum number of bytes that may currently be written to the buffer.
     */
    std::size_t     SpaceAvailable()        const noexcept  { return _Bufsize - _numBytes; }
    
    /** @} */
    
    /**
     @defgroup Accessors Content Access
     @{
     */
    
    /**
     Reads data from the buffer without removing it.
     @param buf A buffer of at least `len` bytes into which the data will be copied.
     @param len The number of bytes to copy. This can be an ideal value; if not
     enough bytes are available, a smaller amount will be copied.
     @result The number of bytes actually copied into `buf`.
     */
    std::size_t     PeekBytes(uint8_t* buf, std::size_t len)    noexcept;
    
    /**
     Reads data from the buffer and removes it, making the space available for writing.
     @param buf A buffer of at least `len` bytes into which the data will be copied.
     @param len The number of bytes to copy. This can be an ideal value; if not
     enough bytes are available, a smaller amount will be copied.
     @result The number of bytes actually copied into `buf`.
     */
    std::size_t     ReadBytes(uint8_t* buf, std::size_t len)    noexcept;
    
    /**
     Writes data into the buffer.
     @param  buf A buffer of at least `len` bytes from which data will be copied.
     @param len The number of bytes to copy. This can be an ideal value; if not
     enough space available, a smaller amount will be copied.
     @result The number of bytes actually copied into the ring buffer.
     */
    std::size_t     WriteBytes(const uint8_t* buf, std::size_t len) noexcept;
    
    /** @} */
    
protected:
    /**
     Implements the copy-out function without regard to locks or updates.
     @param buf The buffer into which to copy the output bytes.
     @param len The number of bytes to copy. It is assumed that this number is
     `<=` _numBytes.
     */
    void                    read_nolock(uint8_t* buf, std::size_t len) noexcept;
    
    /**
     Implements the copy-in function without regard to locks. Updates write position.
     @param buf The buffer from which to copy the output bytes.
     @param len The number of bytes to copy. It is assumed that this number is
     `<=` SpaceAvailable().
     */
    void                    write_nolock(uint8_t* buf, std::size_t len) noexcept;
    
    /**
     Removes a range of bytes without regard to locks. Updates read position.
     
     May also update the write position if all data has been read from the buffer.
     @param len The number of bytes to remove. It is assumed that this number is
     `<=` _numBytes.
     */
    void                    remove_nolock(std::size_t len) noexcept {
        _readPos = ((_readPos + len) % _Bufsize);
    }
    
protected:
    std::array<uint8_t, _Bufsize>   _buffer;    ///< The static buffer backing store.
    
    std::size_t                     _numBytes;  ///< The number of bytes available to read.
    std::size_t                     _readPos;   ///< The current read position.
    std::size_t                     _writePos;  ///< The current write position.
    
    RWLock                          _lock;
    
};

template <std::size_t _Bufsize>
std::size_t RingBuffer<_Bufsize>::PeekBytes(uint8_t* buf, std::size_t len) noexcept
{
    readlock_guard _(_lock);
    
    std::size_t copied = std::min(len, _numBytes);
    if ( copied != 0 )
    {
        read_nolock(buf, copied);
    }
    
    return copied;
}
template <std::size_t _Bufsize>
std::size_t RingBuffer<_Bufsize>::ReadBytes(uint8_t *buf, std::size_t len) noexcept
{
    readlock_guard _(_lock);
    
    std::size_t copied = std::min(len, _numBytes);
    if ( copied != 0 )
    {
        read_nolock(buf, copied);
        remove_nolock(copied);
    }
    
    return copied;
}
template <std::size_t _Bufsize>
std::size_t RingBuffer<_Bufsize>::WriteBytes(const uint8_t *buf, std::size_t len) noexcept
{
    std::lock_guard<RWLock> _(_lock);
    
    std::size_t copied = std::min(len, SpaceAvailable());
    if ( copied != 0 )
    {
        write_nolock(buf, copied);
    }
    
    return copied;
}
template <std::size_t _Bufsize>
void RingBuffer<_Bufsize>::read_nolock(uint8_t *buf, std::size_t len) noexcept
{
    if ( _readPos < _writePos )
    {
        std::memcpy(buf, &_buffer[_readPos], len);
    }
    else
    {
        std::size_t __t = _Bufsize - _readPos;
        std::memcpy(buf, &_buffer[_readPos], __t);
        std::memcpy(&buf[__t], _buffer.data(), len - __t);
    }
}
template <std::size_t _Bufsize>
void RingBuffer<_Bufsize>::write_nolock(uint8_t *buf, std::size_t len) noexcept
{
    if ( _writePos < _readPos )
    {
        std::memcpy(&_buffer[_writePos], buf, len);
        _writePos += len;
    }
    else
    {
        std::size_t __t = _Bufsize - _writePos;
        if ( __t >= len )
        {
            std::memcpy(&_buffer[_writePos], buf, len);
            _writePos += len;
        }
        else
        {
            std::size_t __b = len - __t;
            std::memcpy(&_buffer[_writePos], buf, __t);
            std::memcpy(_buffer.data(), &buf[__t], __b);
            _writePos = __b;
        }
    }
    
    if ( _writePos == _Bufsize )
        _writePos = 0;
    _numBytes += len;
}

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__ring_buffer__) */
