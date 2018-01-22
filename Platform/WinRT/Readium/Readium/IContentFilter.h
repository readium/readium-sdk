//
//  IContentFilter.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-02.
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
