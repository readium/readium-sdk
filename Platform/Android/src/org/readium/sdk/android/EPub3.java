/*
 * EPub3.java
 * ePub3
 *
 * Created by Pedro Reis Colaco (txtr) on 2013-05-29.
 */
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

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import android.util.Log;



/**
 * The ePub3 class implements the interface to the Native
 * ReadiumSDK C++ classes. This stores a reference to the
 * Native object and implements methods to talk with it.
 */
public class EPub3 {

	/*
	 * Static Native library loader
	 */
	static {
		// Load the ePub3 Native lib
		System.loadLibrary("epub3");
	}
	
	
	/*
	 * Constants
	 */

	//TODO: Is this needed at all?
	private static final int BUFFER_SIZE_INCREMENT = 2*1024*1024;
	private static final String TAG = "EPub3";

	
	/**
	 * Private constructor to prevent creating objects
	 * from this class.
	 */
	private EPub3() {}
	
	/*
	 * Methods to be used from native code
	 */
	
	/**
	 * Helper method to create a new String List.
	 * @return The newly created String List.
	 */
	private static List<String> createStringList() {
		return new ArrayList<String>();
	}
	
	/**
	 * Helper method to add a string to a String List.
	 * @param list The String List to add to.
	 * @param str The String to be added.
	 */
	private static void addStringToList(List<String> list, String str) {
		list.add(str);
	}
	
	/**
	 * Helper method to create a Buffer.
	 * @param bufferSize The desired Buffer size.
	 * @return The newly created Buffer.
	 */
	private static ByteBuffer createBuffer(int bufferSize) {
		ByteBuffer buffer = ByteBuffer.allocate(bufferSize);
		buffer.position(0);
		buffer.limit(0);
//		Log.d(TAG, "createBuffer: "+bufferSize);
		return buffer;
	}
	
	/**
	 * Helper method to append bytes to a Buffer.
	 * @param buffer The Buffer to append bytes to.
	 * @param data The data bytes to be appended.
	 */
	private static void appendBytesToBuffer(ByteBuffer buffer, byte[] data) {
		int newLimit = buffer.limit() + data.length;
//		Log.d(TAG, "appendBytesToBuffer: "+newLimit);
		buffer.limit(newLimit);
		buffer.put(data);
	}
	
	
	/*
	 * Static native JNI exported methods
	 */
	
	/**
	 * Sets the core ePub3 SDK cache path, where it can
	 * store temporary files. This needs to be called before
	 * any ePub3 library calls.
	 * @param cachePath The cache path obtained from the
	 * application context.
	 */
	public static native void setCachePath(String cachePath);
	
	/**
	 * Sets a handler that will be called after the filter chain
	 * has been populated. This allows the application to
	 * register any additional filters from within the handler.
	 * This needs to be called before any ePub3 library calls.
	 * @param handler The handler that will be called
	 */
	public static native void setContentFiltersRegistrationHandler(Runnable handler);

	/**
	 * Checks if the supplied book is EPUB3. 
	 * @param path Path to the book.
	 * @return True if the book is EPUB3.
	 */
	public static native boolean isEpub3Book(final String path);

	/**
	 * Open an ePub3 book.
	 * @param path Path to the ePub3 book.
	 * @return A Container object corresponding to the opened
	 * ePub3 book.
	 */
	public static native Container openBook(final String path);
	
	/**
	 * Releases a native pointer from the PointerPool.
	 * @param nativePtr The native pointer.
	 */
	public static native void releaseNativePointer(final long nativePtr);
	
	/*
	 * Helper java methods for handling native calls
	 */
	
	/**
	 * Close an ePub3 book.
	 * @param container The Container object corresponding to
	 * the ePub3 book to close.
	 */
	public static void closeBook(final Container container) {
		container.close();
	}

	public static SdkErrorHandler m_SdkErrorHandler = null;
	public static void setSdkErrorHandler(SdkErrorHandler sdkErrorHandler) {
		m_SdkErrorHandler = sdkErrorHandler;
	}

	private static boolean handleSdkError(String message, boolean isSevereEpubError) {
		if (m_SdkErrorHandler != null) {
			return m_SdkErrorHandler.handleSdkError(message, isSevereEpubError);
		}
			
		System.out.println("!SdkErrorHandler: " + message + " (" + (isSevereEpubError ? "warning" : "info") + ")");

		// never throws an exception
		return true;
	}
}
