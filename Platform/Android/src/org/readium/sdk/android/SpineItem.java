package org.readium.sdk.android;

import org.json.JSONException;
import org.json.JSONObject;

public class SpineItem {
	
	private String idRef;
	private String href;
	private String pageSpread;
	private String renditionLayout;	
	
	public SpineItem(String idRef, String href, String pageSpread, String renditionLayout) {
		
		this.idRef = idRef;
		this.href = href;
		this.pageSpread = pageSpread;
		this.renditionLayout = renditionLayout;
	}
	
	public String getIdRef() {
		return idRef;
	}
	
	public String getHref() {
		return href;
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
		return new JSONObject().put("href", href).put("page_spread", pageSpread).
				put("idref", idRef).put("rendition_layout", renditionLayout);
	}

}
