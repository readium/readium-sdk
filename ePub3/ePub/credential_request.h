//
//  credential_request.h
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

#ifndef ePub3_credential_request_h
#define ePub3_credential_request_h

#if FUTURE_ENABLED

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
        : m_components(o.m_components), m_credentials(), m_promise(), m_pressedButtonIndex(0)
        {}
    CredentialRequest(CredentialRequest&& o)
        : m_components(std::move(o.m_components)), m_credentials(std::move(o.m_credentials)), m_promise(std::move(o.m_promise)), m_pressedButtonIndex(std::move(o.m_pressedButtonIndex))
        {}
    virtual ~CredentialRequest()
        {}
    
	CredentialRequest& operator=(const CredentialRequest&& o)
		{
			m_components = o.m_components;
			m_credentials = o.m_credentials;
			m_promise = promise<Credentials>();
            m_pressedButtonIndex = o.m_pressedButtonIndex;
            return *this;
		}
    CredentialRequest& operator=(CredentialRequest&& o)
        {
            m_components.swap(o.m_components);
            m_credentials.swap(o.m_credentials);
            m_promise.swap(o.m_promise);
            m_pressedButtonIndex = o.m_pressedButtonIndex;
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
        : m_type(type), m_title(title), m_secret(type == Type::MaskedInput), m_default(), m_btnHandler()
            {}
        Component(Type type, string&& title)
        : m_type(type), m_title(title), m_secret(type == Type::MaskedInput), m_default(), m_btnHandler()
            {}
        Component(const Component& o)
        : m_type(o.m_type), m_title(o.m_title), m_secret(o.m_secret), m_default(o.m_default), m_btnHandler(o.m_btnHandler)
            {}
        ~Component() {}
        
    private:
        Type        m_type;
        string      m_title;
        bool        m_secret;
        string      m_default;
        /* added by hslee 15/04/28 */
        ButtonHandler m_btnHandler;
        /* end added*/
        
        friend class CredentialRequest;
        
    };
    
    std::vector<Component>          m_components;
    Credentials                     m_credentials;
    promised_result<Credentials>    m_promise;
    
    /* added by hslee 15/04/13 */
    /* modified by hslee 15/04/28 */
    std::size_t                     m_pressedButtonIndex;
public:
    std::size_t     GetComponentCount() { return m_components.size(); }
    
    void            SetCredentialItem(string title, string input);
    
    future<Credentials>     GetSignal() { return m_promise.get_future(); }
    
    string&                 GetDefaultValue(std::size_t idx);
    
    ButtonHandler           GetButtonHandler(std::size_t idx);
    
    std::size_t             GetPressedButtonIndex(){ return m_pressedButtonIndex; }
    
    void                    SetPressedButtonIndex(std::size_t idx){ m_pressedButtonIndex = idx; }
    /* end added */

};

EPUB3_END_NAMESPACE

#endif //FUTURE_ENABLED

#endif
