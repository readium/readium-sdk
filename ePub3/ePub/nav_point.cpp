//
//  nav_point.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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

#include "nav_point.h"
#include "nav_table.h"
#include "package.h"
#include "../utilities/path_help.h"

EPUB3_BEGIN_NAMESPACE

string NavigationPoint::AbsolutePath(ConstPackagePtr pkg) const
{
    if (_content.empty())
        return string::EmptyString;
    
	string full = pkg->BasePath();

	NavigationElementPtr parent = Owner();
	NavigationTablePtr root;
	do
	{
		if (!bool(parent))
			break;

		root = std::dynamic_pointer_cast<NavigationTable>(parent);
		if (!bool(root))
		{
			NavigationPointPtr pt = std::dynamic_pointer_cast<NavigationPoint>(parent);
			if (!bool(pt))
				break;
			parent = pt->Owner();
		}

	} while (!bool(root));

	if (bool(root))
	{
		string sourceRoot = root->SourceHref();
		
        auto pos = sourceRoot.rfind('/');
		if (pos == string::npos)
            pos = 0;
        
        sourceRoot.erase(pos);

		full += sourceRoot;
	}

    try
    {
        if (_content[0] == '/' && full[full.size() - 1] == '/')
        {
            full += _content.c_str() + 1;
        }
        else if (_content[0] != '/' && full[full.size() - 1] != '/')
        {
            full += '/';
            full += _content;
        }
        else
        {
            full += _content;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

	full = CleanupPath(full);
	auto pos = full.rfind('#');
	if (pos != string::npos)
		full.erase(pos);

	return full;
}

EPUB3_END_NAMESPACE
