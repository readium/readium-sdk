//
//  credential_request.h
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

#ifndef ePub3_credential_request_h
#define ePub3_credential_request_h

#include <ePub3/epub3.h>
#include <ePub3/content_module.h>
#include <map>
#include <future>
#include <vector>

EPUB3_BEGIN_NAMESPACE

typedef std::map<string, string>        Credentials;
typedef Credentials::value_type         CredentialType;

class CredentialRequest
{
public:
    enum class Type : uint8_t
    {
        Message,
        TextInput,
        MaskedInput,
        Button
    };
    
    using ButtonHandler =
        std::function<void(const CredentialRequest*, size_t)>;
    
public:
    CredentialRequest(const string& title,
                      const string& message);
    CredentialRequest(const CredentialRequest& o)
        : m_components(o.m_components), m_credentials(), m_promise()
        {}
    CredentialRequest(CredentialRequest&& o)
        : m_components(std::move(o.m_components)), m_credentials(std::move(o.m_credentials)), m_promise(std::move(o.m_promise))
        {}
    virtual ~CredentialRequest()
        {}
    
	CredentialRequest& operator=(const CredentialRequest&& o)
		{
			m_components = o.m_components;
			m_credentials = o.m_credentials;
			m_promise = promise<Credentials>();
            return *this;
		}
    CredentialRequest& operator=(CredentialRequest&& o)
        {
            m_components.swap(o.m_components);
            m_credentials.swap(o.m_credentials);
            m_promise.swap(o.m_promise);
            return *this;
        }
    
    const string&
    GetTitle() const _NOEXCEPT
        { return GetItemTitle(0); }
    
    const string&
    GetMessage() const _NOEXCEPT
        { return GetItemTitle(1); }
    
    std::size_t
    AddCredential(const string& title,
                  bool secret = false,
                  const string& defaultValue = string::EmptyString);
    std::size_t
    AddButton(const string& title, ButtonHandler&& handler);
    
    Type
    GetItemType(std::size_t idx) const;
    
    const string&
    GetItemTitle(std::size_t idx) const;
    
    const Credentials&
    GetCredentials() const _NOEXCEPT
        { return m_credentials; }
    
    void
    SignalCompletion() _NOEXCEPT
        { m_promise.set_value(m_credentials); }
    
    void
    SignalException(std::exception_ptr exc) _NOEXCEPT
        { m_promise.set_exception(exc); }
    
private:
    class Component
    {
    public:
        Component(Type type, const string& title)
            : m_type(type), m_title(title), m_secret(type == Type::MaskedInput), m_default()
            {}
        Component(Type type, string&& title)
            : m_type(type), m_title(title), m_secret(type == Type::MaskedInput), m_default()
            {}
        Component(const Component& o)
            : m_type(o.m_type), m_title(o.m_title), m_secret(o.m_secret), m_default(o.m_default)
            {}
        ~Component() {}
        
    private:
        Type        m_type;
        string      m_title;
        bool        m_secret;
        string      m_default;
        
        friend class CredentialRequest;
        
    };
    
    std::vector<Component>          m_components;
    Credentials                     m_credentials;
    promised_result<Credentials>    m_promise;
};

EPUB3_END_NAMESPACE

#endif
