//
//  IContentFilter.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-02.
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

#ifndef __Readium_IContentFilter_h__
#define __Readium_IContentFilter_h__

#include "Readium.h"

BEGIN_READIUM_API

ref class Package;
ref class ManifestItem;
interface class IContentFilter;

public delegate bool ContentFilterTypeSniffer(ManifestItem^ item);
public delegate IContentFilter^ ContentFilterFactory(Package^ ownerPackage);

public enum class FilterPriority
{
	/// If you absolutely must see the bytes EXACTLY as they exist in the container, use this priority.
	MustAccessRawBytes			= 1000,

	/// This is the priority at which XML-ENC and XML-DSig filters take place.
	EPUBDecryption				= 750,

	/// This is the priority at which HTML content is modified to process `<switch>` elements and similar.
	SwitchStaticHandling		= 500,

	/// This is the priority at which `<object>` tags may be modified to use EPUB widgets.
	ObjectPreprocessing			= 250,

	/// Any items below this priority level are free to make platform-specific changes. Any validation
	/// will already have taken place by now.
	ValidationComplete			= 100,
};

public interface class IContentFilter
{
	property ContentFilterTypeSniffer^ TypeSniffer { ContentFilterTypeSniffer^ get(); }
	property bool RequiresCompleteData { bool get(); }

	::Platform::Object^ MakeFilterContext(ManifestItem^ forItem);
	::Windows::Storage::Streams::IBuffer^ FilterData(::Platform::Object^ contextInfo, ::Windows::Storage::Streams::IBuffer^ inputData);
};

END_READIUM_API

#endif	/* __Readium_IContentFilter_h__ */
