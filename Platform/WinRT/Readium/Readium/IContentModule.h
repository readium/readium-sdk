//
//  IContentModule.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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

#pragma once

#include "Readium.h"
#include <ePub3/content_module.h>
#include <ePub3/user_action.h>
#include "WinCFI.h"
#include "WinManifest.h"

BEGIN_READIUM_API

ref class Container;

public enum class UserActionType
{
	Display						= int(::ePub3::ActionType::Display),
	Print						= int(::ePub3::ActionType::Print),
	Copy						= int(::ePub3::ActionType::Copy),
	Quote						= int(::ePub3::ActionType::Quote),
	Share						= int(::ePub3::ActionType::Share),
	Highlight					= int(::ePub3::ActionType::Highlight),
	BeginMediaOverlayPlayback	= int(::ePub3::ActionType::BeginMediaOverlayPlayback),
	BeginSpeechSynthesis		= int(::ePub3::ActionType::BeginSpeechSynthesis)
};

public ref class UserAction sealed
{
private:
	const ::ePub3::UserAction&	_native;

internal:
	UserAction(const ::ePub3::UserAction& native)
		: _native(native)
		{}

	property const ::ePub3::UserAction& Native
	{
		const ::ePub3::UserAction& get() { return _native; }
	}

public:
	virtual ~UserAction()
		{}

	property UserActionType Type
	{
		UserActionType get()
		{
			return UserActionType(_native.Type());
		}
	}

	property CFI^ CFI
	{
		Readium::CFI^ get()
		{
			return ref new Readium::CFI(_native.CFI());
		}
	}

	property ::Windows::Foundation::Uri^ IRI
	{
		::Windows::Foundation::Uri^ get()
		{
			return IRIToURI(_native.IRI());
		}
	}

	property ManifestItem^ ManifestItem
	{
		Readium::ManifestItem^ get()
		{
			return Readium::ManifestItem::Wrapper(std::const_pointer_cast<::ePub3::ManifestItem>(_native.ManifestItem()));
		}
	}

};

public interface class IContentModule
{
	::Windows::Foundation::IAsyncOperation<Container^>^ ProcessFile(::Platform::String^ path);
	Container^ ProcessFileSync(::Platform::String^ path);
	void RegisterContentFilters();
	::Windows::Foundation::IAsyncOperation<bool>^ ApproveUserAction(UserAction^ action);

};

END_READIUM_API
