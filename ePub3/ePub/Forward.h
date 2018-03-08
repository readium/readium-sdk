//
//  Forward.h
//  ePub3
//
//  Created by Jim Dovey on 2013-08-26.
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
