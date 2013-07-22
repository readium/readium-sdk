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
	
	@SuppressWarnings("unused")
	private static List<SpineItem> createSpineItemList() {
		return new ArrayList<SpineItem>();
	}
	
	@SuppressWarnings("unused")
	private static SpineItem createSpineItem(String idRef, String href,
			String pageSpread, String renditionLayout) {
		return new SpineItem(idRef, href, pageSpread, renditionLayout);
	}
	
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
	
}
