//
//  nav_point.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
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
