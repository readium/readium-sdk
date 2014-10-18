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
	private final int mLength;
	private int mPos;
	private boolean mClosed = false;
	
	private ResourceInputStream(long nativePtr, int length) {
		__nativePtr = nativePtr;
		mLength = length;
	}
	
	private static ResourceInputStream createResourceInputStream(long nativePtr, long length) {
		return new ResourceInputStream(nativePtr, (int) length);
	}
	
	@Override
	public void close() throws IOException {
		if (!mClosed) {
			super.close();
			nativeReleasePtr(__nativePtr);
			mClosed = true;
		}
	}

	@Override
	public int available() throws IOException {
        return mLength - mPos;
	}

	@Override
	public synchronized int read() throws IOException {
		byte[] buffer = new byte[1];
		if (read(buffer) == 1) {
			return buffer[0];
		}
		// End of file
		return -1;
	}
	
	@Override
	public synchronized long skip(long byteCount) throws IOException {
		nativeSkip(__nativePtr, (int) byteCount);
		return byteCount;
	}

	@Override
	public synchronized int read(byte[] buffer, int offset, int length) {
		ByteBuffer buf = nativeGetBytes(__nativePtr, length);
//        Arrays.checkOffsetAndCount(buffer.length, offset, length);

        // Are there any bytes available?
        if (mPos >= mLength) {
            return -1;
        }
        if (length == 0) {
            return 0;
        }

        int copylen = mLength - mPos < length ? mLength - mPos : length;
        System.arraycopy(buf.array(), 0, buffer, offset, copylen);
        mPos += copylen;
        return copylen;
	}
	
	private native ByteBuffer nativeGetBytes(long nativePtr, int length);
	
	private native void nativeSkip(long nativePtr, int byteCount);
	
	private native void nativeReleasePtr(long nativePtr);
}
