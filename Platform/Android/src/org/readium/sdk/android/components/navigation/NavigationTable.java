package org.readium.sdk.android.components.navigation;

import android.util.Log;

public class NavigationTable extends NavigationElement {

    private static final String TAG = "NavigationTable";
	private final String type;
    private final String sourceHref;

    public NavigationTable(String type, String title, String sourceHref) {
    	super(title);
        this.type = type;
        this.sourceHref = sourceHref;
        Log.i(TAG, "type: "+type+", title: "+title+", sourceHref: "+sourceHref);
    }

    public String getType() {
        return type;
    }

    public String getSourceHref() {
        return sourceHref;
    }
}
