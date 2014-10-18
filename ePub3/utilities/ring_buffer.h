//
//  ring_buffer.h
//  ePub3
//
//  Created by Jim Dovey on 2013-02-05.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __ePub3__ring_buffer__
#define __ePub3__ring_buffer__

#include <ePub3/epub3.h>
#include <ePub3/utilities/basic.h>
#include <mutex>

EPUB3_BEGIN_NAMESPACE

/**
 This class implements a ring buffer.
 
 The function of a ring buffer is that when data is removed, only a read-position
 marker is updated; this means that any remaining data is not copied or moved.  Note
 also that the buffer cannot grow.  This means that you must pay attention to the
 amount of space available in the ring buffer when reading data from any persistent
 storage to be placed herein: only read as much as you can store here.
 
 It is STRONGLY ADVISED that any use of a RingBuffer class be appropiately wrapped
 in calls to its lock() and unlock() methods.  Note that the lock used is a 
 `std::recursive_mutex`, so it is safe to lock it in a nested manner, so long as
 every lock() call is balanced by an unlock().  The RingBuffer class satisfies the
 BasicLockable and Lockable concepts, so it can be locked directly through a
 `std::lock_guard` or `std::unique_lock`, and can be used with a
 `std::condition_variable`, e.g.:
 
     void func(RingBuffer& buf)
     {
         std::lock_guard<RingBuffer> _(buf);
         ...
     }
 
 The ring buffer is used as a component in the AsyncByteStream classes to enable
 asynchronous reading/writing behaviour.
 
 @ingroup utilities
 */
class RingBuffer
{
public:
    ///
    /// Constructs a new RingBuffer instance.
    EPUB3_EXPORT    RingBuffer(std::size_t size=4096);
    ///
    /// Destructor.
    virtual         ~RingBuffer();
    
    ///
    /// Copy constructor (identical input class).
    /// @note This locks its argument before accessing.
    EPUB3_EXPORT    RingBuffer(const RingBuffer& o);
    ///
    /// Move constructor.
    /// @note This locks its argument before accessing.
    EPUB3_EXPORT    RingBuffer(RingBuffer&& o);
    
    /// @{
    /// @name Assignment Operators
    
    /**
     Copy operator.
     @note This locks its parameter before copying.
     */
    EPUB3_EXPORT
    RingBuffer&     operator=(const RingBuffer& o);
    /**
     Move operator.
     @note This locks its parameter before copying.
     */
    EPUB3_EXPORT
    RingBuffer&     operator=(RingBuffer&& o);
    
    /// @}
    
    /// @{
    /**
     @name Locking Operations
     @note The functions here are named such that the RingBuffer class satisfies the
     C++11 Lockable concept. As a result, this object can be used as the `_Mutex`
     template parameter in `std::lock_guard` and `std::unique_lock`.
     */
    
    /**
     Locks the receiver, preventing any modification.
     */
    void            lock()                      { _lock.lock(); }
    
    /**
     Attempts to lock the receiver as per lock().
     @return `true` if the lock was acquired, `false` if it was already locked by
     another thread.
     */
    bool            try_lock()                  { return _lock.try_lock(); }
    
    /**
     Unlocks the receiver, permitting modifications to take place.
     */
    void            unlock()                    { _lock.unlock(); }
    
    /// @}
    
    /// @{
    /// @name Buffer Metadata
    
    /**
     Obtain the total capacity of a ring buffer.
     @result The maximum number of bytes the buffer can hold.
     */
    std::size_t     Capacity()              const _NOEXCEPT  { return _capacity; }
    
    /**
     @return `true` is there is data in the buffer, `false` otherwise.
     */
    bool            HasData()               const _NOEXCEPT  { return _numBytes != 0; }
    
    /**
     @return The number of bytes available to read from the buffer.
     */
    std::size_t     BytesAvailable()        const _NOEXCEPT  { return _numBytes; }
    
    /**
     @return `true` if there is room to write data to the buffer.
     */
    bool            HasSpace()              const _NOEXCEPT  { return _numBytes != _capacity; }
    
    /**
     @return The maximum number of bytes that may currently be written to the buffer.
     */
    std::size_t     SpaceAvailable()        const _NOEXCEPT  { return _capacity - _numBytes; }
    
    /// @}
    
    /// @{
    /// @name Content Accessors
    
    /**
     Reads data from the buffer without removing it.
     @param buf A buffer of at least `len` bytes into which the data will be copied.
     @param len The number of bytes to copy. This can be an ideal value; if not
     enough bytes are available, a smaller amount will be copied.
     @result The number of bytes actually copied into `buf`.
     */
    EPUB3_EXPORT
    std::size_t     ReadBytes(uint8_t* buf, std::size_t len);
    
    /**
     Writes data into the buffer.
     @note This method acquires the instance's modification lock.
     @param  buf A buffer of at least `len` bytes from which data will be copied.
     @param len The number of bytes to copy. This can be an ideal value; if not
     enough space available, a smaller amount will be copied.
     @result The number of bytes actually copied into the ring buffer.
     */
    EPUB3_EXPORT
    std::size_t     WriteBytes(const uint8_t* buf, std::size_t len);
    
    /**
     Removes bytes from the buffer.
     @note This method acquire's the instance's modification lock.
     @param len The number of bytes to remove. When `len > _numBytes` the result is
     undefined.
     */
    EPUB3_EXPORT
    void            RemoveBytes(std::size_t len)    _NOEXCEPT;
    
    /// @}
    
protected:
    std::size_t             _capacity;  ///< The allocated capacity (in bytes) of the backing store.
    uint8_t*                _buffer;    ///< The buffer backing store.
    
    std::size_t             _numBytes;  ///< The number of bytes available to read.
    std::size_t             _readPos;   ///< The current read position.
    std::size_t             _writePos;  ///< The current write position.
    
    std::recursive_mutex    _lock;      ///< An access lock, used to prevent modifications.
    
};



EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__ring_buffer__) */
