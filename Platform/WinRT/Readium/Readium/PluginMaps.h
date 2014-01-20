//
//  PluginMaps.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-02.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __Readium_PluginMaps_h__
#define __Readium_PluginMaps_h__

#include "Readium.h"
#include <ePub3/content_handler.h>
#include <ePub3/filter.h>
#include <unordered_map>
#include <collection.h>

#include "IContentHandler.h"
#include "IContentFilter.h"
#include "CollectionBridges.h"

BEGIN_READIUM_API

//interface class IDRMModule;

class WinRTContentHandler : public ::ePub3::ContentHandler
{
public:
	WinRTContentHandler(IContentHandler^ bridgeInstance);
	virtual ~WinRTContentHandler();

	virtual void operator()(const ::ePub3::string& src, const ParameterList& parameters);

private:
	// ensure at least one strong reference exists!
	IContentHandler^		__bridge_;

};

class WinRTContentFilter : public ::ePub3::ContentFilter
{
public:
	WinRTContentFilter(IContentFilter^ bridgeInstance);
	virtual ~WinRTContentFilter();

	virtual ::ePub3::FilterContext* MakeFilterContext(::ePub3::ConstManifestItemPtr forItem) const;
	virtual bool RequiresCompleteData() const;

	virtual void* FilterData(::ePub3::FilterContext* context, void* data, size_t len, size_t* outLen);

private:
	// ensure at least one strong reference exists!
	IContentFilter^		__bridge_;

};

///////////////////////////////////////////////////////////////////////////////////

ref class ContentHandlerWrapper : IContentHandler
{
	_DECLARE_BRIDGE_API_(::ePub3::ContentHandlerPtr, IContentHandler^);

internal:
	ContentHandlerWrapper(::ePub3::ContentHandlerPtr native);

public:
	virtual ~ContentHandlerWrapper() {}

	property Package^ Owner { virtual Package^ get(); }
	property String^ MediaType { virtual String^ get(); }
	virtual void Invoke(String^ srcPath, ::Windows::Foundation::Collections::IMapView<String^, String^>^ params);

};

ref class ContentFilterWrapper : IContentFilter
{
	_DECLARE_BRIDGE_API_(::ePub3::ContentFilterPtr, IContentFilter^);

internal:
	ContentFilterWrapper(::ePub3::ContentFilterPtr native);

public:
	virtual ~ContentFilterWrapper() {}

	property ContentFilterTypeSniffer^ TypeSniffer { virtual ContentFilterTypeSniffer^ get(); }
	property bool RequiresCompleteData { virtual bool get(); }

	virtual ::Platform::Object^ MakeFilterContext(ManifestItem^ forItem);
	virtual ::Windows::Storage::Streams::IBuffer^ FilterData(::Platform::Object^ contextInfo, ::Windows::Storage::Streams::IBuffer^ inputData);

};

ref class FilterContextWrapper
{
	::ePub3::FilterContext* _native;

internal:
	FilterContextWrapper(::ePub3::FilterContext* native) : _native(native) {}

public:
	virtual ~FilterContextWrapper() { if (_native) delete _native; }

internal:
	property ::ePub3::FilterContext* NativeObject
	{
		::ePub3::FilterContext* get() { return _native; }
	}

};

//////////////////////////////////////////////////////////////////////////////////

::ePub3::ContentHandlerPtr GetNativeContentHandler(IContentHandler^ bridge);
void SetNativeContentHandler(IContentHandler^ bridge, ::ePub3::ContentHandlerPtr native);

::ePub3::ContentFilterPtr GetNativeContentFilter(IContentFilter^ bridge);
void SetNativeContentFilter(IContentFilter^ bridge, ::ePub3::ContentFilterPtr native);

struct ContentHandlerToNative : public std::unary_function<IContentHandler^, ::ePub3::ContentHandlerPtr>
{
	::ePub3::ContentHandlerPtr operator()(IContentHandler^ bridge) const {
		return GetNativeContentHandler(bridge);
	}
};
struct ContentHandlerFromNative : public std::unary_function<::ePub3::ContentHandlerPtr, IContentHandler^>
{
	IContentHandler^ operator()(::ePub3::ContentHandlerPtr native) const {
		return ContentHandlerWrapper::Wrapper(native);
	}
};
struct ContentFilterToNative : public std::unary_function<IContentFilter^, ::ePub3::ContentFilterPtr>
{
	::ePub3::ContentFilterPtr operator()(IContentFilter^ bridge) const {
		return GetNativeContentFilter(bridge);
	}
};
struct ContentFilterFromNative : public std::unary_function<::ePub3::ContentFilterPtr, IContentFilter^>
{
	IContentFilter^ operator()(::ePub3::ContentFilterPtr native) const {
		return ContentFilterWrapper::Wrapper(native);
	}
};

#if EPUB_COMPILER_SUPPORTS(CXX_ALIAS_TEMPLATES)
using BridgedContentHandlerVectorView =
BridgedVectorView<IContentHandler^, ::ePub3::ContentHandlerPtr, ContentHandlerToNative, ContentHandlerFromNative>;
using BridgedContentFilterVectorView =
BridgedVectorView<IContentFilter^, ::ePub3::ContentFilterPtr, ContentFilterToNative, ContentFilterFromNative>;
#else
typedef BridgedVectorView<IContentHandler^, ::ePub3::ContentHandlerPtr, ContentHandlerToNative, ContentHandlerFromNative> BridgedContentHandlerVectorView;
typedef BridgedVectorView<IContentFilter^, ::ePub3::ContentFilterPtr, ContentFilterToNative, ContentFilterFromNative> BridgedContentFilterVectorView;
#endif

END_READIUM_API

#endif	/* __Readium_PluginMaps_h__ */
