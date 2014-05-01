//
//  Forward.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
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

#ifndef ePub3_Forward_h
#define ePub3_Forward_h

#include <ePub3/base.h>
#include <ePub3/utilities/pointer_type.h>
#include <ePub3/utilities/owned_by.h>
#include <memory>

#define _EPUB_DECLARE_CLASS(name) \
class name; \
typedef std::shared_ptr<name> name ## Ptr; \
typedef std::shared_ptr<const name> Const ## name ## Ptr

EPUB3_BEGIN_NAMESPACE

_EPUB_DECLARE_CLASS(Container);
_EPUB_DECLARE_CLASS(Package);
_EPUB_DECLARE_CLASS(ManifestItem);
_EPUB_DECLARE_CLASS(SpineItem);
_EPUB_DECLARE_CLASS(XMLIdentifiable);
_EPUB_DECLARE_CLASS(PropertyHolder);
_EPUB_DECLARE_CLASS(Property);
_EPUB_DECLARE_CLASS(PropertyExtension);
_EPUB_DECLARE_CLASS(ContentFilter);
_EPUB_DECLARE_CLASS(XPathWrangler);
_EPUB_DECLARE_CLASS(CFI);
_EPUB_DECLARE_CLASS(ContentHandler);
_EPUB_DECLARE_CLASS(MediaHandler);
_EPUB_DECLARE_CLASS(EncryptionInfo);
_EPUB_DECLARE_CLASS(DigitalSignature);
_EPUB_DECLARE_CLASS(IRI);
_EPUB_DECLARE_CLASS(ByteStream);
_EPUB_DECLARE_CLASS(Archive);
_EPUB_DECLARE_CLASS(FilterChain);
_EPUB_DECLARE_CLASS(RunLoop);
_EPUB_DECLARE_CLASS(MediaSupportInfo);
_EPUB_DECLARE_CLASS(Collection);
_EPUB_DECLARE_CLASS(Link);

EPUB3_END_NAMESPACE

#endif
