/*
 * Container.java
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

import java.io.File;
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
	
    public static Container openContainer(String path) {
    	// TODO: Implement this
    	return null;
    }
    
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

}
