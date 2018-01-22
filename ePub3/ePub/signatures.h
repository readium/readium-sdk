//
//  signatures.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
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

#ifndef __ePub3__signatures__
#define __ePub3__signatures__

#include <ePub3/epub3.h>

EPUB3_BEGIN_NAMESPACE

class SignatureTransform {};
class SignedInfo {};
class DigestValue {};
class KeyInfo {};
class SignatureObject {};
class SignatureReference {};

/**
 Encapsulates details of a digital signature in an EPUB container.
 
 @todo Implement.
 @ingroup epub-model
 */
class DigitalSignature
#if EPUB_PLATFORM(WINRT)
	: public NativeBridge
#endif
{
public:
    ///
    /// Digital signature algorithms are identified using URI strings.
    typedef string          algorithm_type;
    
public:
                DigitalSignature() : _signedInfo(), _keyInfo(), _object() {}
    /**
     Creates a new DigitalSignature from a `<Signature>` XML element node.
     @param node An XML node, which *must* be a `<Signature>` node as defined in
     XML-DSig.
     @see http://www.w3.org/TR/xmldsig-core1/#sec-Signature
     */
    EPUB3_EXPORT    DigitalSignature(shared_ptr<xml::Node> signatureNode);
    ///
    /// Move constructor.
                    DigitalSignature(DigitalSignature&& o) : _signedInfo(std::move(o._signedInfo)), _keyInfo(std::move(o._keyInfo)), _object(std::move(o._object)) {}
    virtual         ~DigitalSignature() {}

private:
    ///
    /// No copy constructor.
                DigitalSignature(const DigitalSignature&)           _DELETED_;
    ///
    /// No copy assignment operator.
                DigitalSignature&           operator=(const DigitalSignature&)  _DELETED_;

public:
    
    ///
    /// Move assignment operator.
    virtual DigitalSignature&   operator=(DigitalSignature&&);
    
    class SignedInfo*           SignedInfo()                                { return _signedInfo.get(); }
    void                        SetSignedInfo(class SignedInfo& __i)        { _signedInfo.reset(&__i); }
    const class SignedInfo*     SignedInfo()                        const   { return _signedInfo.get(); }
    
    class KeyInfo*              KeyInfo()                                   { return _keyInfo.get(); }
    void                        SetKeyInfo(class KeyInfo& __i)              { _keyInfo.reset(&__i); }
    const class KeyInfo*        KeyInfo()                           const   { return _keyInfo.get(); }
    
    class SignatureObject*      SignatureObject()                           { return _object.get(); }
    void                        SetSignatureObject(class SignatureObject& __i) { _object.reset(&__i); }
    const class SignatureObject* SignatureObject()                  const   { return _object.get(); }
    
    ///
    /// Performs validation of the digital signature.
    EPUB3_EXPORT
    bool                        Validate()                          const;
    
protected:
    unique_ptr<class SignedInfo>        _signedInfo;
    unique_ptr<class KeyInfo>           _keyInfo;
    unique_ptr<class SignatureObject>   _object;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__signatures__) */
