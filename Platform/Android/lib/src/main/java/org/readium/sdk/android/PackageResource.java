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

package org.readium.sdk.android;

import org.readium.sdk.android.util.ResourceInputStream;

import java.io.IOException;
import java.io.InputStream;

public class PackageResource {

    private static final String TAG = "PackageResource";

    private static final int BUFFER_SIZE = 128 * 1024;
    private ResourceInputStream mResourceInputStream;
    private int mContentLength;
    private Package mPackage;
    private String mRelativePath;
    private boolean mHasProperStream;

    public PackageResource(Package mPackage, String mRelativePath) {
        this.mPackage = mPackage;
        this.mRelativePath = mRelativePath;
    }

    /**
     * Reads all data of this resource.
     * @return byte array with the full data
     */
    public byte[] readDataFull() {
        if (!ensureProperByteStream(false)) {
            return null;
        }
        byte[] result = mResourceInputStream.getAllBytes();
        closeByteStream();
        return result;
    }

    /**
     * Reads a range of bytes of this resource.
     * @return byte array with the range data
     */
	public byte[] readDataOfLength(int length, int offset) {
        if (!ensureProperByteStream(true)) {
            return null;
        }
        byte[] result = mResourceInputStream.getRangeBytes(offset, length);
        closeByteStream();
        return result;
    }

    public boolean hasProperStream() {
        return mHasProperStream;
    }

    public InputStream getInputStream(boolean isRangeRequest) {
        if (!ensureProperByteStream(isRangeRequest)) {
            return null;
        }
        return mResourceInputStream;
    }

    public String getRelativePath() {
        return mRelativePath;
    }

    public int getContentLength() {
        return mContentLength;
    }

    private boolean ensureProperByteStream(boolean isRangeRequest){
        if (!mHasProperStream) {
            mResourceInputStream = (ResourceInputStream) mPackage.getInputStream(mRelativePath, BUFFER_SIZE, isRangeRequest);
            if (mResourceInputStream != null) {
                mHasProperStream = true;
                try {
                    mContentLength = mResourceInputStream.available();
                } catch (IOException e) {
                    mHasProperStream = false;
                }
            }
        }
        return mHasProperStream;
    }

    private void closeByteStream() {
        try {
            mResourceInputStream.close();
            mHasProperStream = false;
        } catch (IOException ignored) {
        }
    }
}
