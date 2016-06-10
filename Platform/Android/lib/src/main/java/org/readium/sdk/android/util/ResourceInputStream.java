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

package org.readium.sdk.android.util;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class ResourceInputStream extends InputStream {
	
    private static final String TAG = "ResourceInputStream";
    /**
     * Native Package Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long __nativePtr;
	private boolean mClosed = false;


    private ResourceInputStream(long nativePtr) {
        __nativePtr = nativePtr;
    }

    private static ResourceInputStream createResourceInputStream(long nativePtr) {
        return new ResourceInputStream(nativePtr);
    }
	
	@Override
	public void close() throws IOException {
		if (!mClosed) {
			nativeClose(__nativePtr);
			mClosed = true;
		}
	}

	@Override
	public int available() throws IOException {
        return (int) nativeAvailable(__nativePtr);
	}

	@Override
	public int read() throws IOException {

    	System.err.println("!!! ResourceInputStream int read() ");
        
		byte[] buffer = new byte[1];
		if (read(buffer) == 1) {
			return buffer[0];
		}
		// End of stream
		return -1;
	}

	public byte[] read(int length) {

    	System.err.println("!!! ResourceInputStream int read(int length) ");
        
        return nativeGetBytes(__nativePtr, length);
	}
	
	public long readX(long readLength, byte[] barray) {

    	System.err.println("!!! ResourceInputStream int readX(int length, byte[] barray) ");
        
        return nativeGetBytesX(__nativePtr, readLength, barray);
	}
	
    @Override
    public int read(byte[] buffer, int offset, int length) {

    	System.err.println("!!! ResourceInputStream int read(byte[] buffer, int offset, int length) ");
        
    	byte[] buf = nativeGetBytes(__nativePtr, length);

        if (buf.length > 0) {
            System.arraycopy(buf, 0, buffer, offset, buf.length);
        } else {
            // End of stream
            return -1;
        }
        return buf.length;
    }

    @Override
    public void reset() throws IOException {
    	
    	System.err.println("!!! ResourceInputStream void reset() ");
    	
        nativeReset(__nativePtr, false);
    }

    @Override
    public void mark(int readLimit) {

    	System.err.println("!!! ResourceInputStream void mark(int readLimit) ");
        
        // readLimit ignored
        nativeMark(__nativePtr);
    }

    @Override
    public boolean markSupported() {
        return true;
    }

    @Override
    public long skip(long byteCount) throws IOException {

    	System.err.println("!!! ResourceInputStream long skip(long byteCount) ");
        
        nativeSkip(__nativePtr, (int) byteCount);
        return byteCount;
    }

    public long seek(int position) throws IOException {

    	System.err.println("!!! ResourceInputStream long seek(int position) ");
    	
        nativeReset(__nativePtr, true);
        return this.skip(position);
    }

    /**
     * Reads all data from the stream
     * @return all the bytes available from the stream
     */
    public byte[] getAllBytes() {
        return nativeGetAllBytes(__nativePtr);
    }

    /**
     * Reads a range of bytes from the stream
     * @return bytes from the stream range
     */
    public byte[] getRangeBytes(long offset, long length) {
        return nativeGetRangeBytes(__nativePtr, offset, length);
    }
    
    public long getRangeBytesX(long offset, long length, byte[] barray) {
        return nativeGetRangeBytesX(__nativePtr, offset, length, barray);
    }

	private native byte[] nativeGetBytes(long nativePtr, long readLength);

	private native long nativeGetBytesX(long nativePtr, long readLength, byte[] barray);

    private native byte[] nativeGetAllBytes(long nativePtr);

    private native byte[] nativeGetRangeBytes(long nativePtr, long offset, long length);

	private native long nativeGetRangeBytesX(long nativePtr, long offset, long length, byte[] barray);

	private native void nativeSkip(long nativePtr, long byteCount);

    private native void nativeReset(long nativePtr, boolean ignoreMark);

    private native void nativeMark(long nativePtr);
	
	private native void nativeClose(long nativePtr);

    private native long nativeAvailable(long nativePtr);

}
