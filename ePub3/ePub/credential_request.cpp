//
//  credential_request.cpp
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "credential_request.h"

EPUB3_BEGIN_NAMESPACE

CredentialRequest::CredentialRequest(const string& title, const string& message)
    : m_components(), m_credentials(), m_promise()
{
    m_components.emplace_back(Type::Message, title);
    m_components.emplace_back(Type::Message, message);
}
std::size_t CredentialRequest::AddCredential(const string& title, bool secret,
                                             const string& defaultValue)
{
    std::size_t result = m_components.size();
    m_components.emplace_back((secret ? Type::MaskedInput : Type::TextInput), title);
    if (!defaultValue.empty())
        m_components.back().m_default = defaultValue;
    return result;
}
std::size_t CredentialRequest::AddButton(const string& title, ButtonHandler&& handler)
{
    std::size_t result = m_components.size();
    m_components.emplace_back(Type::Button, title);
    return result;
}
CredentialRequest::Type CredentialRequest::GetItemType(std::size_t idx) const
{
    if (idx >= m_components.size())
        throw std::out_of_range("CredentialRequest::GetItemType");
    return m_components[idx].m_type;
}
const string& CredentialRequest::GetItemTitle(std::size_t idx) const
{
    if (idx >= m_components.size())
        throw std::out_of_range("CredentialRequest::GetItemTitle");
    return m_components[idx].m_title;
}

EPUB3_END_NAMESPACE
