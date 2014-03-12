//
//  IContentModule.h
//  Readium
//
//  Created by Jim Dovey on 2013-11-01.
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