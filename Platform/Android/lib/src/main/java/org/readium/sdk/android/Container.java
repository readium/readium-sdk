/*
 * Container.java
 * ePub3
 *
 * Created by Pedro Reis Colaco (txtr) on 2013-05-29.
 */
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

import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import android.util.Log;

/**
 * The Container class provides an interface for interacting with an EPUB container,
 * i.e. a `.epub` file.
 * <p/>
 * Each Container instance owns all its sub-items. All Packages from a single container,
 * for instance, are kept around as pointers which will be deleted when the container
 * is destroyed.
 *
 * @remarks The Container class holds owning references to the Archive instance used
 * to read from the zip file, the XML document for the OCF file at META-INF/container.xml,
 * all Packages within the container, and all EncryptionInfo instances from
 * META-INF/encryption.xml.
 */
public class Container {

	/**
	 * Log tag
	 */
    private static final String TAG = Container.class.getName();
    
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
	private final long __nativePtr;
	
    /**
     * The container path.
     */
    private final String mPath;
    
    /**
     * List of Packages that belong to this container.
     */
    private final List<Package> mPackages;
    
	/**
	 * Know when we were closed already.
	 */
	private boolean mClosed = false;
	
    /**
     * Container version
     */
    private String mVersion;

    
    /**
     * Constructor. Private to avoid instantiation on the Java side.
     * @param nativePtr Native Container pointer.
     * @param path Path string.
     */
    private Container(long nativePtr, String path) {
    	// Log creation
        Log.i(TAG, "Creating container [ptr:" + String.format("%X", nativePtr) + ", path:" + path + "]");
    	
        // Setup fields
        __nativePtr = nativePtr;
        mPath = path;
        mPackages = new ArrayList<Package>();
    }
    
	@Override
	protected void finalize() {
		// If we are not closed yet?
		if(!mClosed) {
			close();
		}
	}
	
    
	/*
	 * Methods to be used from native code
	 */
	
    /**
     * Container creator method. Just to be used by ePub3 native code. To create
     * a Container, the user should use EPub3.openBook(...)
     * @param nativeContainerPtr Native Container pointer.
     * @param path Path string.
     * @return Created Container object.
     */
    @SuppressWarnings("unused")
	private static Container createContainer(long nativePtr, String path) {
    	Container container = new Container(nativePtr, path);
    	
    	return container;
    }

    @SuppressWarnings("unused")
	private static void addPackageToContainer(Container container, Package pack) {
		container.addPackage(pack);
	}
	

	/*
	 * Public methods
	 */
//
//    public static Container openContainer(String path) {
//    	// TODO: Implement this
//    	return null;
//    }
//
	/**
	 * Closes this container and releases any data of it.
	 */
	public void close() {
		if(!mClosed) {
	    	// Log closing
	        Log.i(TAG, "Closing container [ptr:" + String.format("%X", __nativePtr) + ", path:" + mPath + "]");
	    	
			// Release the native container
			// We do this first to avoid warnings coming from the native pointer
			// pool, due to releasing the native packages pointers while there was
			// still a native pointer in the container pointing to each package.
			// This way the native container is disposed completely and then the
			// packages can be safely disposed without any warning.
			EPub3.releaseNativePointer(__nativePtr);
			
			// Close packages of this container
			for( Package p : mPackages) {
				p.close();
			}
			
			// Set closed
			mClosed = true;
		} else {
			// Log error
			Log.e(TAG, "Closing already closed container [ptr:" + String.format("%X", __nativePtr) + ", path:" + mPath + "]");
		}
	}
	
    /**
     * Returns the native Container pointer.
     * DO NOT USE FROM JAVA SIDE UNLESS TO PASS TO NATIVE CODE!
     * @return Native Container Pointer
     */
	public long getNativePtr() {
		return __nativePtr;
	}
	
	/**
	 * Adds a Package to this container.
	 * @param pack Package to be added.
	 */
	public void addPackage(Package pack) {
		pack.setContainer(this);
		mPackages.add(pack);
	}

    public Package getDefaultPackage() {
        return mPackages.isEmpty() ? null : mPackages.get(0);
    }
    
    public List<Package> getPackages() {
    	return mPackages;
    }

    public String getVersion() {
    	return mVersion;
    }

	public String getPath() {
		return mPath;
	}

	public String getName() {
		return new File(mPath).getName();
	}

	/**
	 * Create a raw InputStream that can be used to fetch content
	 * @param relativePath the location of the resource to load
	 * @return the InputStream. If no data is retrieved, the InputStream is null.
	 */
	public InputStream getInputStream(String relativePath) {
		return this.nativeGetInputStream(this.__nativePtr, relativePath);
	}

	public native InputStream nativeGetInputStream(long nativePtr, String relativePath);
}
