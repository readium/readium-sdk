package org.readium.sdk.android.components.navigation;

import android.util.Log;

public class NavigationPoint extends NavigationElement {

    private static final String TAG = "NavigationPoint";
	private String content;

    public NavigationPoint(String title, String content) {
    	super(title);
        this.content = content;
        Log.i(TAG, "title: "+title+", content: "+content);
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }
}
