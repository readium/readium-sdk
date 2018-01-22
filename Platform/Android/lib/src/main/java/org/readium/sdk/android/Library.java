/*
 * Library.java
 * ePub3
 *
 * Created by Pedro Reis Colaco (txtr) on 2013-08-13.
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

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import android.util.Log;
import android.util.Pair;

/**
 * Java implementation of a library in the smallest sense: it keeps track of ePub
 * files by their unique-identifier, storing the path to that ePub file *as given*.
 * Its primary presence here is to allow for inter-publication linking. It is also
 * designed to be subclassed in order to serve as the basis of a complete library
 * in an application, with all the additional functionality that would involve.
 * 
 * Note that each application would have a single library, accessible only through
 * the getMainLibrary() static method. A library can optionally be written out to
 * disk and then loaded back in by calling getMainLibrary() with a Locator instance
 * at application startup. Once the singleton instance has been created,
 * getMainLibrary() will ignore its argument and always return that instance.
 * 
 * Thoughts: OCF allows for multiple packages to be specified, but I don't see any
 * handling of that in ePub3 CFI?
 */
public class Library {
	
	/**
	 * Log tag
	 */
    protected static final String TAG = Container.class.getName();
    
    /**
     * Static singleton instance of this class.
     */
    protected static Library singleton = null;

    /**
     * Map of known containers.
     */
    protected HashMap<String, Container> mContainers;
    
    /**
     * Map of known packages.
     */
    protected HashMap<String, Pair<String, Package>> mPackages;
    

    /**
     * Create empty Library.
     */
    protected Library() {
		mContainers = new HashMap<String, Container>();
		mPackages = new HashMap<String, Pair<String, Package>>();
    }

	/**
	 * Create a Library and load from a file generated using save().
     * @param path The path of the file to load Library from.
     */
	protected Library(String path) {
		this();
		
		if(!path.isEmpty()) {
			if(!load(path)) {
				Log.e(TAG, "Library(): error loading the library from '" + path + "'");
			}
		}
	}
	

	/*
	 * Public methods
	 */
	
	/**
	 * Access a singleton instance managed by the class and load
	 * it's data from path.
	 * @param path The path of the file to load Library from.
	 * @return The Library singleton.
	 */
	public static Library getMainLibrary(String path) {
    	if(singleton == null) {
    		singleton = new Library(path);
    	}
    	return singleton;
	}
	
	/**
	 * Access an empty singleton instance managed by the class.
	 * @return The Library singleton.
	 */
	public static Library getMainLibrary() { return getMainLibrary(""); }

	/**
	 * Returns the path for the ePub with supplied package unique id.
	 * @param uniqueID The unique id of the package.
	 * @return The path of the ePub.
	 */
	public String getPathForEPubWithUniqueID(String uniqueID) {
		Pair<String, Package> pair = mPackages.get(uniqueID);
		if(pair != null) {
			return pair.first;
		} else {
			return "";
		}
	}
	
	/**
	 * Returns the path for the ePub with supplied package id.
	 * @param packageID The id of the package.
	 * @return The path of the ePub.
	 */
	public String getPathForEPubWithPackageID(String packageID) {
		String uniqueIDStart = packageID + "@";
		
		Iterator<String> iter = mPackages.keySet().iterator();

		while(iter.hasNext()) {
			String key = iter.next();
			if(key.equals(packageID) || key.startsWith(uniqueIDStart)) {
				return mPackages.get(key).first;
			}
		}
		
		return "";
	}

	/**
	 * Adds the publications in the container to the Library.
	 * @param container The container to be added.
	 * @param path The path of the container.
	 */
	public void addPublicationsInContainer(Container container, String path) {
	    // Store the container
		if(!mContainers.containsKey(path))
			mContainers.put(path, container);

		List<Package> packages = container.getPackages();
		
		for(Package pkg : packages) {
			mPackages.put(pkg.getUniqueID(), new Pair<String, Package>(path, pkg));
		}
	}
	
	/**
	 * Adds the publications in the container to the Library.
	 * @param container The container to be added.
	 * @param path The path of the container.
	 */
//	public void addPublicationsInContainerAtPath(String path) {
//	    Container container = Container.openContainer(path);
//	    if(container != null)
//	        addPublicationsInContainer(container, path);
//	}

	/**
	 * Returns an epub3:// url for the given package
	 * @param pack The package
	 * @return An IRI for the given package
	 */
	public IRI getEPubURLForPublication(Package pack) {
		return getEPubURLForPublicationID(pack.getUniqueID());
	}
	
	/**
	 * Returns an epub3:// url for the package with a given identifier
	 * @param packageID The package identifier
	 * @return An IRI for the given package identifier
	 */
	public IRI getEPubURLForPublicationID(String packageID) {
		return new IRI(IRI.EPUBScheme, packageID, "/");
	}

	/**
	 * Gets package for the the supplied url and loads it if allowed.
	 * @param url The url of the package.
	 * @param allowLoad True to perform load.
	 * @return The found package or null if not found.
	 */
//	public Package getPackageForEPubURL(IRI url, boolean allowLoad) {
//	    // Is it an epub URL?
//	    if(url.getScheme() != IRI.EPUBScheme)
//	        return null;
//
//	    String ident = url.getHost();
//	    Pair<String, Package> entry = mPackages.get(ident);
//	    if(entry == null)
//	        return null;
//
//	    if(entry.second != null || !allowLoad )
//	        return entry.second;
//
//	    addPublicationsInContainerAtPath(entry.first);
//
//	    // returns a package ptr or nullptr
//	    return entry.second;
//	}

	/**
	 * Gets package for the the supplied url and loads it.
	 * @param url The url of the package.
	 * @return The found package or null if not found.
	 */
//	public Package getPackageForEPubURL(IRI url) { return getPackageForEPubURL(url, true); }

	/**
	 * Gets the CFI url for a manifest item.
	 * @param item The manifest item.
	 * @return The CFI url.
	 */
	public IRI getEPubCFIURLForManifestItem(ManifestItem item) {
		// TODO: Finish when we have ManifestItem done
	    //IRI packageURL = EPubURLForPublication(item->Owner());
	    //packageURL.SetContentFragmentIdentifier(item->Owner()->CFIForManifestItem(item));
	    //return packageURL;
		return null;
	}

	/**
	 * Gets the manifest item for the CFI.
	 * @param urlWithCFI URL with the CFI.
	 * @param remainingCFI Remaining CFI.
	 * @return The manifest item.
	 */
	ManifestItem getManifestItemForCFI(IRI urlWithCFI, CFI remainingCFI) {
		// TODO: Finish when we have CFI done
	    //CFI cfi = urlWithCFI.ContentFragmentIdentifier();
	    //if ( cfi.Empty() )
	    //    return nullptr;
	    //
	    //PackagePtr pkg = PackageForEPubURL(urlWithCFI);
	    //if ( pkg == nullptr )
	    //    return nullptr;
	    //
	    //return pkg->ManifestItemForCFI(cfi, pRemainingCFI);
		return null;
	}
	
	/*
	 * Overrideable functions to load and save the library.
	 * These are empty and must be overridden to implement desired
	 * storage of the library to the specific needs of the application.
	 */

	/**
	 * Load a library from a file.
	 * Must be overridden.
	 * @param path The path of the Library file.
	 * @return True if load succeeded.
	 */
	protected boolean load(String path) {
		Log.e(TAG, "load(): this method needs to be overrided in a derived Library class.");
		return false;
	}

	/**
	 * Save a library to a file.
	 * Must be overridden.
	 * @param path The path of the Library file.
	 * @return True if save succeeded.
	 */
	public boolean save(String path) {
		Log.e(TAG, "save(): this method needs to be overrided in a derived Library class.");
		return false;
	}
		
}
