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
