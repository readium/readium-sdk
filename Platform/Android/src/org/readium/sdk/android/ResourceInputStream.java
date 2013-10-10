package org.readium.sdk.android;

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
