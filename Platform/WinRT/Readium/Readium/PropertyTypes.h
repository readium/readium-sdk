//
//  PropertyTypes.h
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
