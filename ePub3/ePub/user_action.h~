//
//  user_action.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_user_action_h
#define ePub3_user_action_h

#include <ePub3/ePub3.h>
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

#endif
