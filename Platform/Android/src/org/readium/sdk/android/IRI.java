/*
 * IRI.java
 * ePub3
 *
 * Created by Pedro Reis Colaco (txtr) on 2013-08-13.
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

/**
 * The IRI class encapsulates all URL and URN storage in Readium.
 * 
 * The EPUB 3 specification calls for IRIs rather than URIs (i.e. Unicode characters
 * are allowed and should not be implicitly encoded) in matching properties and other
 * identifiers. This class provides URN support internally, URL support through
 * Google's GURL library, and Unicode IRI support is wrapped around GURL.
 */
public class IRI {
	
	/**
	 * Log tag
	 */
    protected static final String TAG = Container.class.getName();
    
	/**
	 *  The IRI scheme used to refer to EPUB 3 documents.
	 */
	public static final String EPUBScheme = "epub3";
	

    /**
     * Native IRI Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
	private long __nativePtr;
	
	
	/**
    Create a simple URL.
    
    The URL will be in the format:
    
        [scheme]://[host][path]?[query]#[fragment]
    
    If the path is empty or does not begin with a path separator character (`/`),
    one will be inserted automatically.
    @param scheme The URL scheme.
    @param host The host part of the URL.
    @param path The URL's path.
    @param query Any query components of the URL, properly URL-encoded.
    @param fragment A fragmuent used to identify a particular location within a
    resource.
    */
    public IRI(String scheme, String host, String path, String query, String fragment) { }
    public IRI(String scheme, String host, String path, String query) {
    	this(scheme, host, path, query, "");
    }
    public IRI(String scheme, String host, String path) {
    	this(scheme, host, path, "", "");
    }

    public String getScheme() {
    	// TODO: Just for testing now
    	return EPUBScheme;
    }

    public String getHost() {
    	// TODO: Just for testing now
    	return "";
    }

}
