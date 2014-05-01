//
//  WinSMILData.cpp
//  Readium
//
//  Created by Jim Dovey on 2013-11-27.
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

#include "WinSMILData.h"
#include "WinSMILModel.h"
#include "WinManifest.h"
#include "WinSpine.h"

BEGIN_READIUM_API

using ::Platform::String;

ref class TimeNode;
ref class TimeContainer;
ref class Sequence;
ref class Parallel;
ref class Media;
ref class Audio;
ref class Text;

#define _SMILTYPE(_Name) const ::ePub3::SMILData::_Name
#define _SMILPTR(_Name) std::shared_ptr<_SMILTYPE(_Name)>
#define _DECLARE_SUBCLASS_BRIDGE_API_(_Name) \
	internal: \
	property _SMILPTR(_Name) Native ## _Name { _SMILPTR(_Name) get() { return std::dynamic_pointer_cast<_SMILTYPE(_Name)>(NativeObject); } } \
	static _Name^ Wrapper(_SMILPTR(_Name) native)

ref class TimeNode : ISMILTimeNode
{
public:
	_DECLARE_BRIDGE_API_(_SMILPTR(TimeNode), TimeNode^);

internal:
	TimeNode(_SMILPTR(TimeNode) native) : _native(native) {}

public:
	virtual ~TimeNode() {}

	property String^ Name
	{
		virtual String^ get()
		{
			return StringFromNative(_native->Name());
		}
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get();
	}
};

ref class TimeContainer : public TimeNode, ISMILTimeContainer
{
	_DECLARE_SUBCLASS_BRIDGE_API_(TimeContainer);

internal:
	TimeContainer(_SMILPTR(TimeContainer) native) : TimeNode(native) {}

public:
	virtual ~TimeContainer() {}

	property ISMILSequence^ ParentSequence
	{ 
		virtual ISMILSequence^ get();
	}
	property String^ Type
	{ 
		virtual String^ get()
		{
			return StringFromNative(NativeTimeContainer->Type());
		}
	}
	property bool IsParallel
	{
		virtual bool get()
		{
			return NativeTimeContainer->IsParallel();
		}
	}
	property bool IsSequence
	{
		virtual bool get()
		{
			return NativeTimeContainer->IsSequence();
		}
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

ref class Sequence : public TimeContainer, ISMILSequence
{
	_DECLARE_SUBCLASS_BRIDGE_API_(Sequence);

internal:
	Sequence(_SMILPTR(Sequence) native) : TimeContainer(native) {}

public:
	virtual ~Sequence() {}

	property String^ TextRefFile
	{
		virtual String^ get()
		{
			return StringFromNative(NativeSequence->TextRefFile());
		}
	}
	property String^ TextRefFragmentID
	{
		virtual String^ get()
		{
			return StringFromNative(NativeSequence->TextRefFragmentId());
		}
	}
	property ManifestItem^ TextRefManifestItem
	{
		virtual ManifestItem^ get()
		{
			return ManifestItem::Wrapper(NativeSequence->TextRefManifestItem());
		}
	}
	property UINT DurationMilliseconds
	{
		virtual UINT get()
		{
			return static_cast<UINT>(NativeSequence->DurationMilliseconds());
		}
	}

	property UINT ChildCount
	{
		virtual UINT get()
		{
			return static_cast<UINT>(NativeSequence->GetChildrenCount());
		}
	}

	virtual ISMILTimeContainer^ ChildAt(UINT idx)
	{
		return TimeContainer::Wrapper(NativeSequence->GetChild(static_cast<uint32_t>(idx)));
	}

	// TimeContainer
	property ISMILSequence^ ParentSequence
	{
		virtual ISMILSequence^ get() override { return TimeContainer::ParentSequence; }
	}
	property String^ Type
	{
		virtual String^ get() override { return TimeContainer::Type; }
	}
	property bool IsParallel
	{
		virtual bool get() override { return TimeContainer::IsParallel; }
	}
	property bool IsSequence
	{
		virtual bool get() override { return TimeContainer::IsSequence; }
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

ref class Media : public TimeNode, ISMILMedia
{
	_DECLARE_SUBCLASS_BRIDGE_API_(Media);

internal:
	Media(_SMILPTR(Media) native) : TimeNode(native) {}

public:
	virtual ~Media() {}

	property ISMILParallel^ ParentParallel
	{
		virtual ISMILParallel^ get();
	}
	property String^ SrcFile
	{
		virtual String^ get()
		{
			return StringFromNative(NativeMedia->SrcFile());
		}
	}
	property String^ SrcFragmentID
	{
		virtual String^ get()
		{
			return StringFromNative(NativeMedia->SrcFragmentId());
		}
	}
	property ManifestItem^ SrcManifestItem
	{
		virtual ManifestItem^ get()
		{
			return ManifestItem::Wrapper(NativeMedia->SrcManifestItem());
		}
	}
	property bool IsAudio
	{
		virtual bool get()
		{
			return NativeMedia->IsAudio();
		}
	}
	property bool IsText
	{
		virtual bool get()
		{
			return NativeMedia->IsText();
		}
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

ref class Audio : public Media, ISMILAudio
{
	_DECLARE_SUBCLASS_BRIDGE_API_(Audio);

internal:
	Audio(_SMILPTR(Audio) native) : Media(native) {}

public:
	virtual ~Audio() {}

	property UINT ClipBeginMilliseconds
	{
		virtual UINT get()
		{
			return static_cast<UINT>(NativeAudio->ClipBeginMilliseconds());
		}
	}
	property UINT ClipEndMilliseconds
	{
		virtual UINT get()
		{
			return static_cast<UINT>(NativeAudio->ClipEndMilliseconds());
		}
	}
	property UINT ClipDurationMilliseconds
	{
		virtual UINT get()
		{
			return static_cast<UINT>(NativeAudio->ClipDurationMilliseconds());
		}
	}

	// Media
	property ISMILParallel^ ParentParallel
	{
		virtual ISMILParallel^ get() override { return Media::ParentParallel; }
	}
	property String^ SrcFile
	{
		virtual String^ get() override { return Media::SrcFile; }
	}
	property String^ SrcFragmentID
	{
		virtual String^ get() override { return Media::SrcFragmentID; }
	}
	property ManifestItem^ SrcManifestItem
	{
		virtual ManifestItem^ get() override { return Media::SrcManifestItem; }
	}
	property bool IsAudio
	{
		virtual bool get() override { return Media::IsAudio; }
	}
	property bool IsText
	{
		virtual bool get() override { return Media::IsText; }
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

ref class Text : public Media, ISMILText
{
	_DECLARE_SUBCLASS_BRIDGE_API_(Text);

internal:
	Text(_SMILPTR(Text) native) : Media(native) {}

public:
	virtual ~Text() {}

	// Media
	property ISMILParallel^ ParentParallel
	{
		virtual ISMILParallel^ get() override { return Media::ParentParallel; }
	}
	property String^ SrcFile
	{
		virtual String^ get() override { return Media::SrcFile; }
	}
	property String^ SrcFragmentID
	{
		virtual String^ get() override { return Media::SrcFragmentID; }
	}
	property ManifestItem^ SrcManifestItem
	{
		virtual ManifestItem^ get() override { return Media::SrcManifestItem; }
	}
	property bool IsAudio
	{
		virtual bool get() override { return Media::IsAudio; }
	}
	property bool IsText
	{
		virtual bool get() override { return Media::IsText; }
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

ref class Parallel : public TimeContainer, ISMILParallel
{
	_DECLARE_SUBCLASS_BRIDGE_API_(Parallel);

internal:
	Parallel(_SMILPTR(Parallel) native) : TimeContainer(native) {}

public:
	virtual ~Parallel() {}

	property ISMILAudio^ Audio
	{
		virtual ISMILAudio^ get()
		{
			typedef ref class Audio _Cls;
			return _Cls::Wrapper(NativeParallel->Audio());
		}
	}
	property ISMILText^ Text
	{
		virtual ISMILText^ get()
		{
			typedef ref class Text _Cls;
			return _Cls::Wrapper(NativeParallel->Text());
		}
	}

	// TimeContainer
	property ISMILSequence^ ParentSequence
	{
		virtual ISMILSequence^ get() override { return TimeContainer::ParentSequence; }
	}
	property String^ Type
	{
		virtual String^ get() override { return TimeContainer::Type; }
	}
	property bool IsParallel
	{
		virtual bool get() override { return TimeContainer::IsParallel; }
	}
	property bool IsSequence
	{
		virtual bool get() override { return TimeContainer::IsSequence; }
	}

	// TimeNode
	property String^ Name
	{
		virtual String^ get() override { return TimeNode::Name; }
	}
	property ISMILTimeContainer^ Parent
	{
		virtual ISMILTimeContainer^ get() override { return TimeNode::Parent; }
	}
};

_BRIDGE_API_IMPL_(_SMILPTR(TimeNode), TimeNode)
_BRIDGE_API_IMPL_(_SMILPTR(TimeContainer), TimeContainer)
_BRIDGE_API_IMPL_(_SMILPTR(Sequence), Sequence)
_BRIDGE_API_IMPL_(_SMILPTR(Media), Media)
_BRIDGE_API_IMPL_(_SMILPTR(Audio), Audio)
_BRIDGE_API_IMPL_(_SMILPTR(Text), Text)
_BRIDGE_API_IMPL_(_SMILPTR(Parallel), Parallel)
_BRIDGE_API_IMPL_(::ePub3::SMILDataPtr, SMILData)

ISMILTimeContainer^ TimeNode::Parent::get()
{
	return TimeContainer::Wrapper(_native->Parent());
}

ISMILSequence^ TimeContainer::ParentSequence::get()
{
	return Sequence::Wrapper(NativeTimeContainer->ParentSequence());
}

ISMILParallel^ Media::ParentParallel::get()
{
	return Parallel::Wrapper(NativeMedia->ParentParallel());
}

ISMILParallel^ SMILData::WrappedParallel(_SMILPTR(Parallel) native)
{
	return Parallel::Wrapper(native);
}
SpineItem^ SMILData::XhtmlSpineItem::get()
{
	return SpineItem::Wrapper(_native->XhtmlSpineItem());
}
ManifestItem^ SMILData::SmilManifestItem::get()
{
	return ManifestItem::Wrapper(_native->SmilManifestItem());
}
ISMILSequence^ SMILData::Body::get()
{
	return Sequence::Wrapper(_native->Body());
}
UINT SMILData::DurationMilliseconds_Metadata::get()
{
	return static_cast<UINT>(_native->DurationMilliseconds_Metadata());
}
UINT SMILData::DurationMilliseconds_Calculated::get()
{
	return static_cast<UINT>(_native->DurationMilliseconds_Calculated());
}

END_READIUM_API
