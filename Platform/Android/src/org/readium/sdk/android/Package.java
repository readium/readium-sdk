/*
 * Package.java
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

import java.io.InputStream;
import java.nio.ByteBuffer;
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
        Log.i(TAG, "Creating package [ptr:" + String.format("%X", nativePtr) + "]");
    	
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
	        Log.i(TAG, "Closing package [ptr:" + String.format("%X", __nativePtr) + "]");
	    	
			//TODO: cleanup data
			
			// Release the native package
			EPub3.releaseNativePointer(__nativePtr);
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
		tableOfContents = nativeGetTableOfContents(__nativePtr);
		listOfFigures = nativeGetListOfFigures(__nativePtr);
		listOfIllustrations = nativeGetListOfIllustrations(__nativePtr);
		listOfTables = nativeGetListOfTables(__nativePtr);
		pageList = nativeGetPageList(__nativePtr);
		manifestTable = nativeGetManifestTable(__nativePtr);
		Log.i(TAG, "package nativePtr: " + __nativePtr);
		Log.i(TAG, "title: "+title);
		Log.i(TAG, "subtitle: "+subtitle);
		Log.i(TAG, "shortTitle: "+shortTitle);
		Log.i(TAG, "collectionTitle: "+collectionTitle);
		Log.i(TAG, "editionTitle: "+editionTitle);
		Log.i(TAG, "expandedTitle: "+expandedTitle);
		Log.i(TAG, "fullTitle: "+fullTitle);
		Log.i(TAG, "uniqueID: "+uniqueID);
		Log.i(TAG, "urlSafeUniqueID: "+urlSafeUniqueID);
		Log.i(TAG, "packageID: "+packageID);
		Log.i(TAG, "basePath: "+basePath);
		Log.i(TAG, "type: "+type);
		Log.i(TAG, "version: "+version);
		Log.i(TAG, "isbn: "+isbn);
		Log.i(TAG, "language: "+language);
		Log.i(TAG, "copyrightOwner: "+copyrightOwner);
		Log.i(TAG, "source: "+source);
		Log.i(TAG, "authors: "+authors);
		Log.i(TAG, "authorList: "+authorList);
		Log.i(TAG, "modificationDate: "+modificationDate);
		Log.i(TAG, "pageProgressionDirection: "+pageProgressionDirection);
		Log.i(TAG, "subjects: "+subjects);
		Log.i(TAG, "spineItems: "+spineItems.size());
		Log.i(TAG, "tableOfContents: "+tableOfContents);
		Log.i(TAG, "listOfFigures: "+listOfFigures);
		Log.i(TAG, "listOfIllustrations: "+listOfIllustrations);
		Log.i(TAG, "listOfTables: "+listOfTables);
		Log.i(TAG, "pageList: "+pageList);
		Log.i(TAG, "manifestTable: "+manifestTable.size());
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

	public List<String> getSubjects() {
		return subjects;
	}

	public List<SpineItem> getSpineItems() {
		return spineItems;
	}

	public NavigationTable getTableOfContents() {
		return tableOfContents;
	}

	public NavigationTable getListOfFigures() {
		return listOfFigures;
	}

	public NavigationTable getListOfIllustrations() {
		return listOfIllustrations;
	}

	public NavigationTable getListOfTables() {
		return listOfTables;
	}

	public NavigationTable getPageList() {
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
		for (ManifestItem item : manifestTable) {
			if (relativePath.equals(item.getHref())) {
				return item;
			}
		}
		return null;
	}

	/**
	 * Fetch the whole file into a byte array.
	 * @param relativePath the location of the resource to load
	 * @return the resource as a byte array. If no data is retrieved, the byte array length is 0.
	 */
	public byte[] getContent(String relativePath) {
		ByteBuffer buffer = nativeReadStreamForRelativePath(__nativePtr, container.getNativePtr(), relativePath);
		if (buffer == null) {
			return new byte[0];
		}
		byte[] content = new byte[buffer.limit()];
		System.arraycopy(buffer.array(), 0, content, 0, content.length);
		return content;
	}

	/**
	 * Create an InputStream that .
	 * @param relativePath the location of the resource to load
	 * @return the InputStream. If no data is retrieved, the InputStream is null.
	 */
	public InputStream getInputStream(String relativePath) {
		return nativeInputStreamForRelativePath(__nativePtr, container.getNativePtr(), relativePath);
	}

	/**
	 * This method is useful to know the size of a specific resource 
	 * without having to actually load that resource.
	 * @param relativePath the location of the resource
	 * @return the size of the file or -1 if no resource match the path
	 */
	public int getArchiveInfoSize(String relativePath) {
		return nativeGetArchiveInfoSize(__nativePtr, container.getNativePtr(), relativePath);
	}

	/**
	 * Convert the package to JSON object.
	 * @return representation of the package to be consumed by the Readium JS library.
	 */
	public JSONObject toJSON() {
		JSONObject o = new JSONObject();
		try {
			o.put("rootUrl", basePath);
			o.put("rendition_layout", nativeGetProperty(__nativePtr, "layout", "rendition"));
			JSONArray spineArray = new JSONArray();
			for (SpineItem item : spineItems) {
				spineArray.put(item.toJSON());
			}
			JSONObject spine = new JSONObject();
			spine.put("items", spineArray);
			spine.put("direction", pageProgressionDirection);
			o.put("spine", spine);
			o.put("mediaOverlays", new JSONArray());
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
	
	/*
	 * Navigation tables
	 */
	private native NavigationTable nativeGetTableOfContents(long nativePtr);
	private native NavigationTable nativeGetListOfFigures(long nativePtr);
	private native NavigationTable nativeGetListOfIllustrations(long nativePtr);
	private native NavigationTable nativeGetListOfTables(long nativePtr);
	private native NavigationTable nativeGetPageList(long nativePtr);
	private native List<ManifestItem> nativeGetManifestTable(long nativePtr);
	
	/*
	 * Content 
	 */
	private native InputStream nativeInputStreamForRelativePath(long nativePtr, 
			long containerPtr, String relativePath);
	private native ByteBuffer nativeReadStreamForRelativePath(long nativePtr, 
			long containerPtr, String relativePath);
	
	private native int nativeGetArchiveInfoSize(long nativePtr, 
			long containerPtr, String relativePath);

}
