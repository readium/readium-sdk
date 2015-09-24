/*
 * Package.java
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

import java.io.InputStream;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

import org.readium.sdk.android.components.navigation.NavigationTable;


/**
 * TODO
 */
public class Package {
	private static final String TAG = "Package";
	
    /**
     * Native Package Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
	private final long __nativePtr;

	/**
	 * Know when we were closed already.
	 */
	private boolean mClosed = false;
	
	//TODO: Make all these final changeable only from native code
	private Container container;
	private String title;
	private String subtitle;
	private String shortTitle;
	private String collectionTitle;
	private String editionTitle;
	private String expandedTitle;
	private String fullTitle;
	private String uniqueID;
	private String urlSafeUniqueID;
	private String packageID;
	private String basePath;
	private String type;
	private String version;
	private String isbn;
	private String language;
	private String copyrightOwner;
	private String source;
	private String authors;
	private String modificationDate;
	private String pageProgressionDirection;
	private String rendition_layout;
	private String rendition_flow;
	private String rendition_orientation;
	private String rendition_spread;
	private String smilDataJson;
	private List<String> authorList;
	private List<String> subjects;
	private List<SpineItem> spineItems;
	private NavigationTable tableOfContents;
	private NavigationTable listOfFigures;
	private NavigationTable listOfIllustrations;
	private NavigationTable listOfTables;
	private NavigationTable pageList;

	private List<ManifestItem> manifestTable;


	private Package(long nativePtr) {
    	// Log creation
        Log.d(TAG, "Creating package [ptr:" + String.format("%X", nativePtr) + "]");
		__nativePtr = nativePtr;
//        Log.i(TAG, "package nativePtr: "+nativePtr);
        loadData();
	}
	
	@Override
	protected void finalize() {
		// If we are not closed yet?
		if(!mClosed) {
			close();
		}
	}
	
	private static Package createPackage(long nativePtr) {
		Package pack = new Package(nativePtr);
		
		return pack;
	}
	
	/**
	 * Closes this package and releases any data of it.
	 */
	public void close() {
		if(!mClosed) {
	    	// Log closing
	        Log.d(TAG, "Closing package [ptr:" + String.format("%X", __nativePtr) + "]");
	    	
			//TODO: cleanup data
			
			// Release the native package
			EPub3.releaseNativePointer(__nativePtr);
			
			// Set closed
			mClosed = true;
		} else {
			// Log error
			Log.e(TAG, "Closing already closed package [ptr:" + String.format("%X", __nativePtr) + "]");
		}
	}
	
	public void setContainer(Container container) {
		this.container = container;
	}

	//TODO: Pass this to native code
	private void loadData() {
		title = nativeGetTitle(__nativePtr);
		subtitle = nativeGetSubtitle(__nativePtr);
		shortTitle = nativeGetShortTitle(__nativePtr);
		collectionTitle = nativeGetCollectionTitle(__nativePtr);
		editionTitle = nativeGetEditionTitle(__nativePtr);
		expandedTitle = nativeGetExpandedTitle(__nativePtr);
		fullTitle = nativeGetFullTitle(__nativePtr);
		uniqueID = nativeGetUniqueID(__nativePtr);
		urlSafeUniqueID = nativeGetURLSafeUniqueID(__nativePtr);
		packageID = nativeGetPackageID(__nativePtr);
		basePath = nativeGetBasePath(__nativePtr);
		type = nativeGetType(__nativePtr);
		version = nativeGetVersion(__nativePtr);
		isbn = nativeGetISBN(__nativePtr);
		language = nativeGetLanguage(__nativePtr);
		copyrightOwner = nativeGetCopyrightOwner(__nativePtr);
		source = nativeGetSource(__nativePtr);
		authors = nativeGetAuthors(__nativePtr);
		modificationDate = nativeGetModificationDate(__nativePtr);
		pageProgressionDirection = nativeGetPageProgressionDirection(__nativePtr);
		authorList = nativeGetAuthorList(__nativePtr);
		subjects = nativeGetSubjects(__nativePtr);
		spineItems = nativeGetSpineItems(__nativePtr);
		smilDataJson = nativeGetSmilDataAsJson(__nativePtr);
		rendition_layout = nativeGetProperty(__nativePtr, "layout", "rendition");
		rendition_flow = nativeGetProperty(__nativePtr, "flow", "rendition");
		rendition_orientation = nativeGetProperty(__nativePtr, "orientation", "rendition");
		rendition_spread = nativeGetProperty(__nativePtr, "spread", "rendition");
		Log.d(TAG, "package nativePtr: " + __nativePtr);
		Log.d(TAG, "title: "+title);
		Log.d(TAG, "subtitle: "+subtitle);
		Log.d(TAG, "shortTitle: "+shortTitle);
		Log.d(TAG, "collectionTitle: "+collectionTitle);
		Log.d(TAG, "editionTitle: "+editionTitle);
		Log.d(TAG, "expandedTitle: "+expandedTitle);
		Log.d(TAG, "fullTitle: "+fullTitle);
		Log.d(TAG, "uniqueID: "+uniqueID);
		Log.d(TAG, "urlSafeUniqueID: "+urlSafeUniqueID);
		Log.d(TAG, "packageID: "+packageID);
		Log.d(TAG, "basePath: "+basePath);
		Log.d(TAG, "type: "+type);
		Log.d(TAG, "version: "+version);
		Log.d(TAG, "isbn: "+isbn);
		Log.d(TAG, "language: "+language);
		Log.d(TAG, "copyrightOwner: "+copyrightOwner);
		Log.d(TAG, "source: "+source);
		Log.d(TAG, "authors: "+authors);
		Log.d(TAG, "authorList: "+authorList);
		Log.d(TAG, "modificationDate: "+modificationDate);
		Log.d(TAG, "pageProgressionDirection: "+pageProgressionDirection);
		Log.d(TAG, "subjects: "+subjects);
		Log.d(TAG, "spineItems: "+spineItems.size());
//		Log.d(TAG, "smilDataJson: "+ smilDataJson);
		Log.d(TAG, "rendition_layout: "+rendition_layout);
		Log.d(TAG, "rendition_flow: "+rendition_flow);
		Log.d(TAG, "rendition_orientation: "+rendition_orientation);
		Log.d(TAG, "rendition_spread: "+rendition_spread);
	}

	public long getNativePtr() {
		return __nativePtr;
	}

	public String getTitle() {
		return title;
	}

	public String getSubtitle() {
		return subtitle;
	}

	public String getShortTitle() {
		return shortTitle;
	}

	public String getCollectionTitle() {
		return collectionTitle;
	}

	public String getEditionTitle() {
		return editionTitle;
	}

	public String getExpandedTitle() {
		return expandedTitle;
	}

	public String getFullTitle() {
		return fullTitle;
	}

	public String getUniqueID() {
		return uniqueID;
	}

	public String getUrlSafeUniqueID() {
		return urlSafeUniqueID;
	}

	public String getPackageID() {
		return packageID;
	}
	
	public String getBasePath() {
		return basePath;
	}

	public String getType() {
		return type;
	}

	public String getVersion() {
		return version;
	}

	public String getIsbn() {
		return isbn;
	}

	public String getLanguage() {
		return language;
	}

	public String getCopyrightOwner() {
		return copyrightOwner;
	}

	public String getSource() {
		return source;
	}

	public String getAuthors() {
		return authors;
	}

	public List<String> getAuthorList() {
		return authorList;
	}

	public String getModificationDate() {
		return modificationDate;
	}

	public String getPageProgressionDirection() {
		return pageProgressionDirection;
	}

	public String getRenditionLayout() {
		return rendition_layout;
	}
	
	public String getRenditionFlow() {
		return rendition_flow;
	}
	
	public String getRenditionOrientation() {
		return rendition_orientation;
	}
	
	public String getRenditionSpread() {
		return rendition_spread;
	}

	public List<String> getSubjects() {
		return subjects;
	}

	/**
	 * This method is left for backward compatibility, just in case some people access it.
	 * This is why the structure is lazily initialized.
	 * @return The list of {@link org.readium.sdk.android.ManifestItem}.
	 */
	@Deprecated
    public List<ManifestItem> getManifestTable() {
		if (manifestTable == null)
			manifestTable = nativeGetManifestTable(__nativePtr);
		return manifestTable;
    }

	public List<SpineItem> getSpineItems() {
		return spineItems;
	}
	
	public SpineItem getSpineItem(String idref) {
		for (SpineItem si : spineItems) {
			if (si.getIdRef().equals(idref)) {
				return si;
			}
		}
		return null;
	}

	public NavigationTable getTableOfContents() {
		if (tableOfContents == null && !mClosed) {
			tableOfContents = nativeGetTableOfContents(__nativePtr);
			Log.d(TAG, "tableOfContents: "+tableOfContents);
		}
		return tableOfContents;
	}

	public NavigationTable getListOfFigures() {
		if (listOfFigures == null && !mClosed) {
			listOfFigures = nativeGetListOfFigures(__nativePtr);
			Log.d(TAG, "listOfFigures: "+listOfFigures);
		}
		return listOfFigures;
	}

	public NavigationTable getListOfIllustrations() {
		if (listOfIllustrations == null && !mClosed) {
			listOfIllustrations = nativeGetListOfIllustrations(__nativePtr);
			Log.d(TAG, "listOfIllustrations: "+listOfIllustrations);
		}
		return listOfIllustrations;
	}

	public NavigationTable getListOfTables() {
		if (listOfTables == null && !mClosed) {
			listOfTables = nativeGetListOfTables(__nativePtr);
			Log.d(TAG, "listOfTables: "+listOfTables);
		}
		return listOfTables;
	}

	public NavigationTable getPageList() {
		if (pageList == null && !mClosed) {
			pageList = nativeGetPageList(__nativePtr);
			Log.d(TAG, "pageList: "+pageList);
		}
		return pageList;
	}

	/**
	 * Returns true if a manifest item is found AND its media type is "application/xhtml+xml".
	 * Returns false otherwise.
	 * @param relativePath needed to find a manifest item
	 * @return
	 */
	public boolean isHtml(String relativePath) {
		ManifestItem manifestItem = getManifestItem(relativePath);
		if (manifestItem != null) {
			return manifestItem.isHtml();
		}
		return false;
	}

	/**
	 * Returns a manifest item if one is found.
	 * Returns null if not found.
	 * @param relativePath needed to find a manifest item
	 * @return a manifest item or null
	 */
	public ManifestItem getManifestItem(String relativePath) {
		return nativeGetManifestItemByRelativePath(__nativePtr, relativePath);
	}

    /**
     * Returns a manifest item if one is found.
     * Returns null if not found.
     * @param id needed to find a manifest item
     * @return a manifest item or null
     */
    public ManifestItem getManifestItemById(String id) {
		return nativeGetManifestItemById(__nativePtr, id);
    }

    public PackageResource getResourceAtRelativePath(String relativePath) {
        return new PackageResource(this, relativePath);
    }

    /**
     * Create an InputStream that can be used to fetch content.
     * @param relativePath the location of the resource to load
     * @param isRangeRequest if this is a range request or not
     * @return the InputStream. If no data is retrieved, the InputStream is null.
     */
    public InputStream getInputStream(String relativePath, boolean isRangeRequest) {
        return getInputStream(relativePath, 0, isRangeRequest);
    }

    /**
     * Create a raw InputStream that can be used to fetch content without using content filters.
     * @param relativePath the location of the resource to load
     * @return the InputStream. If no data is retrieved, the InputStream is null.
     */
    public InputStream getRawInputStream(String relativePath) {
        return getRawInputStream(relativePath, 0);
    }

    /**
     * Create an InputStream that can be used to fetch content.
	 * @param relativePath the location of the resource to load
	 * @param isRangeRequest if this is a range request or not
	 * @return the InputStream. If no data is retrieved, the InputStream is null.
	 */
	public InputStream getInputStream(String relativePath, int bufferSize, boolean isRangeRequest) {
		// Avoid native crash
        if (mClosed) {
            return null;
        }
		return nativeInputStreamForRelativePath(__nativePtr, container.getNativePtr(), relativePath, bufferSize, isRangeRequest);
	}

    /**
     * Create a raw InputStream that can be used to fetch content without using content filters.
     * @param relativePath the location of the resource to load
     * @return the InputStream. If no data is retrieved, the InputStream is null.
     */
    public InputStream getRawInputStream(String relativePath, int bufferSize) {
		// Avoid native crash
		if (mClosed) {
			return null;
		}
        return nativeRawInputStreamForRelativePath(__nativePtr, container.getNativePtr(), relativePath, bufferSize);
    }

	/**
	 * This method is useful to know the size of a specific resource 
	 * without having to actually load that resource.
	 * @param relativePath the location of the resource
	 * @return the size of the file or -1 if no resource match the path
	 */
	public int getArchiveInfoSize(String relativePath) {
		// Avoid native crash
        if (mClosed) {
            return 0;
        }
		return nativeGetArchiveInfoSize(__nativePtr, container.getNativePtr(), relativePath);
	}

    public String getProperty(String name) {
        return getProperty(name, "");
    }

    private String getProperty(String name, String prefix) {
		// Avoid native crash
        if (mClosed) {
            return null;
        }
        return nativeGetProperty(__nativePtr, name, prefix);
    }

    public boolean containsProperty(String name, String prefix) {
		// Avoid native crash
        if (mClosed) {
            return false;
        }
        return nativeContainsProperty(__nativePtr, name, prefix);
    }

	// sensible legacy defaults
	private String _rootUrl = "/";
	private String _rootUrlMO = "http://127.0.0.1:8080/";

	public void setRootUrls(String rootUrl, String rootUrlMO) {
		_rootUrl = rootUrl;
		_rootUrlMO = rootUrlMO;
	}
	
	/**
	 * Convert the package to JSON object.
	 * @return representation of the package to be consumed by the Readium JS library.
	 */
	public JSONObject toJSON() {
		JSONObject o = new JSONObject();
		try {
			o.put("rootUrl", _rootUrl);
			if (_rootUrlMO != null) {
				o.put("rootUrlMO", _rootUrlMO);
			}

			o.put("rendition_layout", rendition_layout);
			o.put("rendition_flow", rendition_flow);
			o.put("rendition_orientation", rendition_orientation);
			o.put("rendition_spread", rendition_spread);
			JSONArray spineArray = new JSONArray();
			for (SpineItem item : spineItems) {
				spineArray.put(item.toJSON());
			}
			JSONObject spine = new JSONObject();
			spine.put("items", spineArray);
			spine.put("direction", pageProgressionDirection);
			o.put("spine", spine);

            JSONObject mo = new JSONObject(smilDataJson);
            o.put("media_overlay", mo);
			
//			Log.i(TAG, "JSON: " + o.toString(2));
		} catch (JSONException e) {
			Log.e(TAG, "" + e.getMessage(), e);
		}
		return o;
	}

	/*
	 * Miscellaneous book metadata
	 */
	private native String nativeGetTitle(long nativePtr);
	private native String nativeGetSubtitle(long nativePtr);
	private native String nativeGetShortTitle(long nativePtr);
	private native String nativeGetCollectionTitle(long nativePtr);
	private native String nativeGetEditionTitle(long nativePtr);
	private native String nativeGetExpandedTitle(long nativePtr);
	private native String nativeGetFullTitle(long nativePtr);
	private native String nativeGetUniqueID(long nativePtr);
	private native String nativeGetURLSafeUniqueID(long nativePtr);
	private native String nativeGetPackageID(long nativePtr);
	private native String nativeGetBasePath(long nativePtr);
	private native String nativeGetType(long nativePtr);
	private native String nativeGetVersion(long nativePtr);
	private native String nativeGetISBN(long nativePtr);
	private native String nativeGetLanguage(long nativePtr);
	private native String nativeGetCopyrightOwner(long nativePtr);
	private native String nativeGetSource(long nativePtr);
	private native String nativeGetAuthors(long nativePtr);
	private native String nativeGetModificationDate(long nativePtr);
	private native String nativeGetPageProgressionDirection(long nativePtr);
	private native List<String> nativeGetAuthorList(long nativePtr);
	private native List<String> nativeGetSubjects(long nativePtr);
	private native List<SpineItem> nativeGetSpineItems(long nativePtr);
	private native String nativeGetProperty(long nativePtr, String propertyName, String prefix);
	private native boolean nativeContainsProperty(long nativePtr, String propertyName, String prefix);

	/*
	 * Navigation tables
	 */
	private native NavigationTable nativeGetTableOfContents(long nativePtr);
	private native NavigationTable nativeGetListOfFigures(long nativePtr);
	private native NavigationTable nativeGetListOfIllustrations(long nativePtr);
	private native NavigationTable nativeGetListOfTables(long nativePtr);
	private native NavigationTable nativeGetPageList(long nativePtr);
	private native List<ManifestItem> nativeGetManifestTable(long nativePtr);
	private native ManifestItem nativeGetManifestItemByRelativePath(long nativePtr, String relativePath);
	private native ManifestItem nativeGetManifestItemById(long nativePtr, String id);

	/*
	 * Content 
	 */
	private native InputStream nativeInputStreamForRelativePath(long nativePtr, 
			long containerPtr, String relativePath, int bufferSize, boolean isRange);

    private native InputStream nativeRawInputStreamForRelativePath(long nativePtr,
            long containerPtr, String relativePath, int bufferSize);

    private native int nativeGetArchiveInfoSize(long nativePtr,
			long containerPtr, String relativePath);

	/*
	 * SMIL
	 */
	private native String nativeGetSmilDataAsJson(long nativePtr);
}
