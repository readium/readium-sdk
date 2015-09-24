/*
 * ManifestItem.java
 * SDKLauncher-Android
 *
 * Created by Yonathan Teitelbaum (Mantano) on 2013-09-02.
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

public class ManifestItem {

	public enum ItemProperties {

		None                (0),        ///< No properties defined.
		CoverImage          (1<<0),     ///< This item is the cover image for the publication.
		ContainsMathML      (1<<1),     ///< This item contains MathML markup.
		Navigation          (1<<2),     ///< This item is the EPUB 3 navigation document for the publication.
		HasRemoteResources  (1<<3),     ///< This item accesses resources located outside the EPUB container.
		HasScriptedContent  (1<<4),     ///< This item has content which uses JavaScript or HTML5 forms.
		ContainsSVG         (1<<5),     ///< This item contains SVG markup.
		ContainsSwitch      (1<<6),     ///< This item contains an `epub:switch` element.

		AllPropertiesMask   ((1<<7)-1); ///< A mask for all standard EPUB 3 properties.
		private final int flag;

		ItemProperties(int flag) {

			this.flag = flag;
		}
	}

	private final long __nativePtr;
	private final String mId;
	/**
	 * Location, relative to the manifestation's Package document.
	 */
	private final String mHref;
	/**
	 * Media-type, usually a MIME type (although EPUB may define new values for
	 * items which do not have canonical MIME types assigned).
	 */
	private final String mMediaType;

	public ManifestItem(long nativePtr, String id, String href, String mediaType) {
		__nativePtr = nativePtr;
		mId = id;
		mHref = href;
		mMediaType = mediaType;
	}

	public String getId() {
		return mId;
	}

	public String getHref() {
		return mHref;
	}

	public String getMediaType() {
		return mMediaType;
	}
	
	public boolean isCover() {
		return hasProperty(ItemProperties.CoverImage);
	}

	public boolean isMathml() {
		return hasProperty(ItemProperties.ContainsMathML);
	}

	public boolean isNavigation() {
		return hasProperty(ItemProperties.Navigation);
	}

	public boolean hasRemoteResources() {
		return hasProperty(ItemProperties.HasRemoteResources);
	}

	public boolean hasScriptedContent() {
		return hasProperty(ItemProperties.HasScriptedContent);
	}

	public boolean containsSVG() {
		return hasProperty(ItemProperties.ContainsSVG);
	}

	public boolean containsSwitch() {
		return hasProperty(ItemProperties.ContainsSwitch);
	}

	private boolean hasProperty(ItemProperties itemProperty) {
		return nativeHasProperty(__nativePtr, itemProperty.flag);
	}

	private native boolean nativeHasProperty(long nativePtr, int flag);

	/**
	 * Returns true if the manifest item is an HTML resource.
	 * Returns false otherwise.
	 * @return true if the media type is exactly "application/xhtml+xml".
	 */
	public boolean isHtml() {
		return "application/xhtml+xml".equals(mMediaType);
	}

	@Override
	public String toString() {
		return "ManifestItem [href=" + mHref + ", mediaType=" + mMediaType + "]";
	}
	
}
