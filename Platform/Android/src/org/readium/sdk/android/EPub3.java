/*
 * EPub3.java
 * ePub3
 *
 * Created by Pedro Reis Colaco (txtr) on 2013-05-29.
 * Copyright (c) 2012-2013 The Readium Foundation and contributors.
 * 
 * The Readium SDK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package org.readium.sdk.android;

import java.nio.ByteBuffer;


/**
 * The ePub3 class implements the interface to the Native
 * ReadiumSDK C++ classes. This stores a reference to the
 * Native object and implements methods to talk with it.
 * 
 * @author Pedro Reis Colaco
 */
public class EPub3 {

	/*
     * Private member fields
     */

	/**
	 * TODO: Reference to NATIVE object. Don't use directly from
	 * Java.
	 */
	private ByteBuffer __Native;


	/*
	 * Static Native library loader
	 */
	static {
		// Load the ePub3 Native lib
        System.loadLibrary("epub3");
	}
	
	/*
	 * Constructors
	 */
	public EPub3() {
		// Initialize private member fields
		__Native = null; //TODO: Setup native object (TBD)
	}
	
    /*
     * Native JNI exported methods
     */

	/**
	 * Open an epub book.
	 * @param path Path to the epub book.
	 * @return A handle to the opened epub book.
	 */
	public final native int openBook(final String path);
	
	/**
	 * Closed a previously openned epub book.
	 * @param handle The handle to the epub book to close.
	 */
    public final native void closeBook(final int handle);

}
