//
//  path_help.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-11-19.
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

#include "path_help.h"
#include <sstream>
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

string CleanupPath(const string& path)
{
	static REGEX_NS::regex _PathSplitter("/");
	std::vector<string> components = path.split(_PathSplitter);

	auto begin = components.begin();
	auto end = components.end();
	for (auto pos = begin; pos < end; ++pos)
	{
		if (*pos == ".." && pos != begin)
		{
			decltype(pos) dotDot, parent;

			dotDot = pos--;
			if (pos == begin)
			{
				parent = begin;
				pos = dotDot;
			}
			else
			{
				parent = pos--;
			}

			components.erase(parent, dotDot);
			end = components.end();
		}
	}

	std::ostringstream ss;
	for (auto& str : components)
	{
		ss << str;
		ss << '/';
	}

	string result = ss.str();
	if (path[path.size() - 1] != '/')
		result.erase(result.size() - 1);

	return result;
}

EPUB3_END_NAMESPACE
