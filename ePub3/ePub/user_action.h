//
//  user_action.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
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

#ifndef ePub3_user_action_h
#define ePub3_user_action_h

#if FUTURE_ENABLED

#include <ePub3/epub3.h>
#include <ePub3/cfi.h>
#include <ePub3/utilities/iri.h>

EPUB3_BEGIN_NAMESPACE

enum class ActionType
{
    Display                     = 0,
    Print                       = 1,
    Copy                        = 2,
    Quote                       = 3,
    Share                       = 4,
    Highlight                   = 5,
	BeginMediaOverlayPlayback   = 6,
	BeginSpeechSynthesis        = 7,
    BeginAudioMediaPlayback     = 8,
    BeginVideoMediaPlayback     = 9,
    BeginAnimation              = 10
};

class UserAction
{
public:
    UserAction(ConstManifestItemPtr item,
               const CFI& cfi,
               ActionType type=ActionType::Display)
        : m_item(item), m_cfi(cfi), m_type(type)
        {}
    virtual ~UserAction()
        {}
    
    virtual
    ActionType           Type()         const _NOEXCEPT FINAL { return m_type; };
    virtual
    const class CFI&     CFI()          const _NOEXCEPT FINAL { return m_cfi; }
    virtual
    ConstManifestItemPtr ManifestItem() const _NOEXCEPT FINAL { return m_item; }
    virtual
    const IRI&           IRI()          const _NOEXCEPT FINAL { return m_iri; }
    
private:
    ActionType              m_type;
    class CFI               m_cfi;
    class IRI               m_iri;
    ConstManifestItemPtr    m_item;
};

EPUB3_END_NAMESPACE


#endif //FUTURE_ENABLED

#endif
