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
	 * @param pageSpread
	 * @param renditionLayout
	 * @return a spine item
	 */
	@SuppressWarnings("unused")
	private static SpineItem createSpineItem(String idRef, String href,
			String pageSpread, String renditionLayout) {
		return new SpineItem(idRef, href, pageSpread, renditionLayout);
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
