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
	private String renditionFlow;
	private String renditionOrientation;
	private String renditionSpread;
    private boolean linear;
	private String mediaOverlayId;

    public SpineItem(String idRef, String title, String href, String mediaType,
			String pageSpread, String renditionLayout, String renditionFlow,
            String renditionOrientation, String renditionSpread, boolean linear,
            String mediaOverlayId) {

		this.idRef = idRef;
		this.title = title;
		this.href = href;
		this.mediaType = mediaType;
		this.pageSpread = pageSpread;
		this.renditionLayout = renditionLayout;
		this.renditionFlow = renditionFlow;
		this.renditionOrientation = renditionOrientation;
		this.renditionSpread = renditionSpread;
        this.linear = linear;
		this.mediaOverlayId = mediaOverlayId;
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

	public String getRenditionLayout(Package pack) {
        String layout = renditionLayout;
    	if (pack != null && (layout == null || layout.length() == 0)) {
    		layout = pack.getRenditionLayout();
    	}
        return layout;
	}

    public String getRenditionFlow(Package pack) {
        String flow = renditionFlow;
    	if (pack != null && (flow == null || flow.length() == 0)) {
    		flow = pack.getRenditionFlow();
    	}
        return flow;
    }

    public String getRenditionOrientation(Package pack) {
        String orientation = renditionOrientation;
    	if (pack != null && (orientation == null || orientation.length() == 0)) {
    		orientation = pack.getRenditionOrientation();
    	}
        return orientation;
    }

    public String getRenditionSpread(Package pack) {
    	String spread = renditionSpread;
    	if (pack != null && (spread == null || spread.length() == 0)) {
    		spread = pack.getRenditionSpread();
    	}
        return spread;
    }

    public boolean isLinear() {
        return linear;
    }

    public boolean isFixedLayout(Package pack) {
    	String layout = getRenditionLayout(pack);
		return "pre-paginated".equals(layout);
	}

	public JSONObject toJSON() throws JSONException {
		return new JSONObject()
                .put("href", href)
                .put("media_type", mediaType)
                .put("page_spread", pageSpread)
                .put("idref", idRef)
                .put("rendition_layout", renditionLayout)
                .put("rendition_flow", renditionFlow)
                .put("rendition_orientation", renditionOrientation)
                .put("rendition_spread", renditionSpread)
                .put("linear", linear ? "yes": "no")
                .put("media_overlay_id", mediaOverlayId);
	}

}
