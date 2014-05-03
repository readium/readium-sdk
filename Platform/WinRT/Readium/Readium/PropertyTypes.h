//
//  PropertyTypes.h
//  Readium
//
//  Created by Jim Dovey on 2013-10-11.
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

#pragma once

#include "Readium.h"

BEGIN_READIUM_API

/// Enumerated constants for the DCMES metadata attributes used by EPUB 3.
public enum class DCType
{
	/// An invalid value.
	Invalid,

	// Required DCMES Elements.

	/// A unique identifier.
	Identifier,
	/// The title of the publication.
	Title,
	/// The language in which the publication is rendered.
	Language,

	// Optional DCMES elements

	/// The name of a secondary contributor.
	Contributor,
	/// Spatial or temporal context of the publication.
	Coverage,
	/// The name of a primary creator.
	Creator,
	/// The original publication date (*not* the modification date).
	Date,
	/// A description of the publication's contents.
	Description,
	/// Information on the format in which the publication is rendered.
	Format,
	/// Details of the publisher of the work.
	Publisher,
	/// An identifier for a related resource.
	Relation,
	/// Information about rights associated with the publication.
	Rights,
	/// An identifier for the source of this EPUB, e.g. the ISBN of an original print edition.
	Source,
	/// The topic or topics which form the subject of the publication.
	Subject,
	/// The type of content encoded in this publication, e.g. book, annotations, minutes, etc.
	Type,

	/// A non-DCMES metadata value, identified only by its property IRI.
	Custom = UCHAR_MAX
};

/// Enumeration for page spreads.
public enum class PageSpread
{
	/// No value specified by the author.
	Automatic,
	/// This is the left page of a spread.
	Left,
	/// This is the right page of a spread.
	Right,
	/// This is a double-width page, spread across both left & right.
	Center,
};

/// Enumeration for page progression directions.
public enum class PageProgression
{
	/// Assume based on language, etc.
	Default,
	/// Pages flow from left to right, as in English.
	LeftToRight,
	/// Pages flow from right to left, as in Japanese comics & vertical text.
	RightToLeft,
};

END_READIUM_API
