//
//  signatures.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
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
