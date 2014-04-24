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

import org.json.JSONException;
import org.json.JSONObject;

public class SpineItem {
	
	private String idRef;
	private String title;
	private String href;
	private String mediaType;
	private String pageSpread;
	private String renditionLayout;
	private String media_overlay_id;
	
	public SpineItem(String idRef, String title, String href, String mediaType, 
			String pageSpread, String renditionLayout, String media_overlay_id) {
		
		this.idRef = idRef;
		this.title = title;
		this.href = href;
		this.mediaType = mediaType;
		this.pageSpread = pageSpread;
		this.renditionLayout = renditionLayout;
		this.media_overlay_id = media_overlay_id;
	}
	
	public String getIdRef() {
		return idRef;
	}
	
	public String getTitle() {
		return title;
	}
	
	public String getHref() {
		return href;
	}
	
	public String getMediaType() {
		return mediaType;
	}

	public String getPageSpread() {
		return pageSpread;
	}
	
	public String getRenditionLayout() {
		return renditionLayout;
	}
	
	public boolean isFixedLayout() {
		return "pre-paginated".equals(renditionLayout);
	}

	public JSONObject toJSON() throws JSONException {
		return new JSONObject().put("href", href).put("media_type", mediaType).put("page_spread", pageSpread).
				put("idref", idRef).put("rendition_layout", renditionLayout).put("media_overlay_id", media_overlay_id);
	}

}
