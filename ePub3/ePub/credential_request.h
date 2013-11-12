//
//  credential_request.h
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
			m_promise = std::promise<Credentials>();
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
