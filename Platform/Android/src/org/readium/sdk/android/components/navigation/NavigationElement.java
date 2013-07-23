package org.readium.sdk.android.components.navigation;

import java.util.ArrayList;
import java.util.List;

public class NavigationElement {

    private final String title;

    protected final List<NavigationElement> children;

    public NavigationElement(String title) {
		this.title = title;
		children = new ArrayList<NavigationElement>();
	}

	public String getTitle() {
        return title;
    }

    public List<NavigationElement> getChildren() {
        return children;
    }

    public void appendChild(NavigationElement e) {
        children.add(e);
    }
}
