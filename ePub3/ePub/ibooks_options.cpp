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

#include "ibooks_options.h"
#include "archive.h"
#include "archive_xml.h"

#include <ePub3/xml/document.h>
#include <ePub3/xml/element.h>
#include <ePub3/xml/node.h>
#include <ePub3/xml/io.h>
#include <ePub3/xml/xmlstring.h>
#include <set>
#include <mutex>		// for once_flag and call_once

#define THROW_INVALID_XML()	throw std::invalid_argument("iBooksOptions: XML file does not conform to described format.")

EPUB3_BEGIN_NAMESPACE

const string iBooksOptions::AppleMetadataPath("META-INF/com.apple.ibooks.display-options.xml");

iBooksOptions::iBooksOptions(unique_ptr<ArchiveXmlReader>&& reader)
	: _platforms()
{
	auto doc = reader->ReadDocument(nullptr, nullptr, xml::PROHIBIT_DTD);
	if (doc == nullptr)
		throw std::invalid_argument("iBooksOptions: No document loaded from reader.");

	auto root = doc->Root();
	if (root == nullptr || root->Name() != _XSTR("display_options"))
		THROW_INVALID_XML();

	for (auto platform = root->FirstElementChild();
		 platform != nullptr; 
		 platform = platform->NextElementSibling())
	{
		if (platform->Name() != _XSTR("platform"))
			THROW_INVALID_XML();

		static const xml::string			kNameAttr(_XSTR("name"));
		static const xml::string			kEmptyNamespace(_XSTR(""));
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
		static const std::set<xml::string>	kSupportedPlatformNames{_XSTR("*"), _XSTR("iphone"), _XSTR("ipad")};
#else
		static std::set<xml::string>		kSupportedPlatformNames;
		static std::once_flag __once;
		std::call_once(__once, [&]() {
			kSupportedPlatformNames.insert(_XSTR("*"));
			kSupportedPlatformNames.insert(_XSTR("iphone"));
			kSupportedPlatformNames.insert(_XSTR("ipad"));
		});
#endif

		xml::string platformName = platform->AttributeValue(kNameAttr, kEmptyNamespace);
		if (kSupportedPlatformNames.find(platformName) == kSupportedPlatformNames.end())
			THROW_INVALID_XML();

		option_type options;

		for (auto option = platform->FirstElementChild();
			 option != nullptr;
			 option = option->NextElementSibling())
		{
			if (option->Name() != _XSTR("option"))
				THROW_INVALID_XML();

			string name = option->AttributeValue(kNameAttr, kEmptyNamespace);
			if (name.empty())
				THROW_INVALID_XML();

			string value = option->Content();
#if EPUB_HAVE(CXX_MAP_EMPLACE)
			options.emplace(std::move(name), std::move(value));
#else
			options[name] = value;
#endif
		}

		if (options.empty())
			continue;

#if EPUB_HAVE(CXX_MAP_EMPLACE)
		_platforms.emplace(platformName, std::move(options));
#else
		_platforms[platformName] = options;
#endif
	}
}

const string& iBooksOptions::GetOptionValue(const string& option, const string& platform) const
{
	if (_platforms.empty())
		return string::EmptyString;

	auto options = _platforms.find(string(platform));
	if (options == _platforms.end() && platform == "*")
		options = _platforms.begin();

	if (options == _platforms.end())
		return string::EmptyString;

	auto value = options->second.find(option);
	if (value == options->second.end())
		return string::EmptyString;

	return value->second;
}

EPUB3_END_NAMESPACE
