package org.readium.sdk.android;

import org.json.JSONException;
import org.json.JSONObject;

public class SpineItem {
	
	private String idRef;
	private String title;
	private String href;
	private String pageSpread;
	private String renditionLayout;
	private String media_overlay_id;
	
	public SpineItem(String idRef, String title, String href, String pageSpread, String renditionLayout, String media_overlay_id) {
		
		this.idRef = idRef;
		this.title = title;
		this.href = href;
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
				put("idref", idRef).put("rendition_layout", renditionLayout).put("media_overlay_id", media_overlay_id);
	}

}
