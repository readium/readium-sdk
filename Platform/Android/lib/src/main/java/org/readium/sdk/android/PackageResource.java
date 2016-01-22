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
