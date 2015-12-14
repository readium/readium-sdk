//
//  credential_request.cpp
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
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
    
    //hslee
    if(handler != nullptr)
        m_components.back().m_btnHandler = handler;
    
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

/* added by hslee 15/04/13 */
void CredentialRequest::SetCredentialItem(string title, string input)
{
    m_credentials.insert(std::make_pair(title, input));
}

string& CredentialRequest::GetDefaultValue(std::size_t idx)
{
    if (idx >= m_components.size())
        throw std::out_of_range("CredentialRequest::GetItemTitle");
    return m_components[idx].m_default;
}

CredentialRequest::ButtonHandler
CredentialRequest::GetButtonHandler(std::size_t idx)
{
    if (idx >= m_components.size())
        throw std::out_of_range("CredentialRequest::GetItemTitle");
    return m_components[idx].m_btnHandler;
}
/* end added */


EPUB3_END_NAMESPACE
