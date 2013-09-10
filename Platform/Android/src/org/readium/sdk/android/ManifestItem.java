/*
 * ManifestItem.java
 * SDKLauncher-Android
 *
 * Created by Yonathan Teitelbaum (Mantano) on 2013-09-02.
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

public class ManifestItem {

	/**
	 * Location, relative to the manifestation's Package document.
	 */
	private final String mHref;
	/**
	 * Media-type, usually a MIME type (although EPUB may define new values for
	 * items which do not have canonical MIME types assigned).
	 */
	private final String mMediaType;

	public ManifestItem(String href, String mediaType) {
		mHref = href;
		mMediaType = mediaType;
	}

	public String getHref() {
		return mHref;
	}

	public String getMediaType() {
		return mMediaType;
	}
	
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
