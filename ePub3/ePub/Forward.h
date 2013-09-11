//
//  Forward.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_Forward_h
#define ePub3_Forward_h

#include <epub3/base.h>
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

EPUB3_END_NAMESPACE

#endif
