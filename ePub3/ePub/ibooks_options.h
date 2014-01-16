//
//  ibooks_options.h
//  ePub3
//
//  Created by Jim Dovey on 2014-01-15.
//  Copyright (c) 2012-2014 The Readium Foundation and contributors.
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

#ifndef __ePub3__ibooks_options__
#define __ePub3__ibooks_options__

#include <ePub3/epub3.h>
#include <ePub3/container.h>
#include <map>

EPUB3_BEGIN_NAMESPACE

class Container;

/**
Contains all properties and options defined by the iBooks option file in a Container.

Note that this class is only used internally by the Container class. If iBooks options
are present in a container, then the container will use those options to set the appropriate
EPUB3 properties on all relevant packages. The iBooksOptions instance will then be discarded.

@ingroup epub-model
*/
class iBooksOptions
{
private:
	typedef std::map<string, string>		option_type;
	typedef std::map<string, option_type>	platforms_type;

	platforms_type							_platforms;

public:
	static const string AppleMetadataPath;

public:
	iBooksOptions()
		: _platforms()
		{}

	iBooksOptions(unique_ptr<ArchiveXmlReader>&& reader);

	iBooksOptions(const iBooksOptions& __o)
		: _platforms(__o._platforms)
		{}
	iBooksOptions(iBooksOptions&& __o)
		: _platforms(std::move(__o._platforms))
		{}

	~iBooksOptions()
		{}

	void swap(iBooksOptions& __o) _NOEXCEPT
		{ _platforms.swap(__o._platforms); }

	const string& GetOptionValue(const string& option, const string& platform = string("*")) const;

};

EPUB3_END_NAMESPACE

#endif	/* __ePub3__ibooks_options__ */
