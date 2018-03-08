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

import java.util.ArrayList;
import java.util.List;

import org.readium.sdk.android.SpineItem;
import org.readium.sdk.android.components.navigation.NavigationElement;
import org.readium.sdk.android.components.navigation.NavigationPoint;
import org.readium.sdk.android.components.navigation.NavigationTable;

/**
 * This class is a helper accessed by the native code. It centralizes all the
 * creations of Java objects in a unique place.
 * It is final+abstract and all its methods are private in order to avoid using
 * it from Java code (JNI does not enforce access control restrictions :-)
 * @author Mantano
 *
 * TODO: Remove this class and pass the creators and adders to the respective
 * classes as it was done for general methods (went to EPub3 class), Container
 * and Package.
 */
public abstract class JavaObjectsFactory {
	
	/**
	 * Creates a new list of SpineItem
	 * @return the created list
	 */
	@SuppressWarnings("unused")
	private static List<SpineItem> createSpineItemList() {
		return new ArrayList<SpineItem>();
	}
	
	/**
	 * Creates a new SpineItem
	 * @param idRef spine item identifier
	 * @param href 
	 * @param mediaType
	 * @param pageSpread
	 * @param renditionLayout
     * @param linear
	 * @param media_overlay_id
	 * @return a spine item
	 */
	@SuppressWarnings("unused")
	private static SpineItem createSpineItem(String idRef, String title, String href, String mediaType,
			String pageSpread, String renditionLayout, String renditionFlow, String renditionOrientation, String renditionSpread, boolean linear, String media_overlay_id) {
		return new SpineItem(idRef, title, href, mediaType, pageSpread, renditionLayout, renditionFlow, renditionOrientation, renditionSpread, linear, media_overlay_id);
	}

	/**
	 * Adds an spine item to the list.
	 * @return the created list
	 */
	@SuppressWarnings("unused")
	private static void addSpineItemToList(List<SpineItem> list, SpineItem spineItem) {
		list.add(spineItem);
	}
	
	@SuppressWarnings("unused")
	private static NavigationTable createNavigationTable(String type, String title, String sourceHref) {
		return new NavigationTable(type, title, sourceHref);
	}
	
	@SuppressWarnings("unused")
	private static NavigationPoint createNavigationPoint(String title, String content) {
		return new NavigationPoint(title, content);
	}
	
	@SuppressWarnings("unused")
	private static void addElementToParent(NavigationElement parent, NavigationElement child) {
		parent.appendChild(child);
	}
	
	/**
	 * Creates a new list of ManifestItem
	 * @return the created list
	 */
	@SuppressWarnings("unused")
	private static List<ManifestItem> createManifestItemList() {
		return new ArrayList<ManifestItem>();
	}
	
	/**
	 * Creates a new ManifestItem
	 * @param href
	 * @param mediaType
	 * @return manifest item
	 */
	@SuppressWarnings("unused")
	private static ManifestItem createManifestItem(String href, String mediaType) {
		return new ManifestItem(href, mediaType);
	}

	/**
	 * Adds a manifest item to the list.
	 * @return the created list
	 */
	@SuppressWarnings("unused")
	private static void addManifestItemToList(List<ManifestItem> list, ManifestItem manifestItem) {
		list.add(manifestItem);
	}
	
}
