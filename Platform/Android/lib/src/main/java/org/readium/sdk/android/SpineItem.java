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
