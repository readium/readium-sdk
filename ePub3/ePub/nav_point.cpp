//
//  nav_point.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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
