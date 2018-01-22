//
//  NavElementList.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#include "WinNavTable.h"
#include "WinNavPoint.h"
#include "CollectionBridges.h"

BEGIN_READIUM_API

struct NavElementToNative : public std::unary_function<INavigationElement^, ::ePub3::NavigationElementPtr>
{
	::ePub3::NavigationElementPtr operator()(INavigationElement^ input) {
		NavigationPoint^ pt = dynamic_cast<NavigationPoint^>(input);
		if (pt != nullptr)
			return std::dynamic_pointer_cast<::ePub3::NavigationElement>(pt->NativeObject);
		NavigationTable^ tb = dynamic_cast<NavigationTable^>(input);
		if (tb != nullptr)
			return std::dynamic_pointer_cast<::ePub3::NavigationElement>(tb->NativeObject);
		return nullptr;
	}
};
struct NavElementFromNative : public std::unary_function<::ePub3::NavigationElementPtr, INavigationElement^>
{
	INavigationElement^ operator()(::ePub3::NavigationElementPtr native)
	{
		auto pt = std::dynamic_pointer_cast<::ePub3::NavigationPoint>(native);
		if (bool(pt))
			return NavigationPoint::Wrapper(pt);
		auto tb = std::dynamic_pointer_cast<::ePub3::NavigationTable>(native);
		if (bool(tb))
			return NavigationTable::Wrapper(tb);
		return nullptr;
	}
};

using NavElementList = BridgedVectorView<INavigationElement^, ::ePub3::NavigationElementPtr, NavElementToNative, NavElementFromNative>;

END_READIUM_API
