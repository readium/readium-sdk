//
//  signatures.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-31.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__signatures__
#define __ePub3__signatures__

#include "epub3.h"

EPUB3_BEGIN_NAMESPACE

class SignatureTransform {};
class SignedInfo {};
class DigestValue {};
class KeyInfo {};
class SignatureObject {};
class SignatureReference {};

class DigitalSignature
{
public:
    typedef std::string     algorithm_type;
    
public:
    DigitalSignature() = default;
    DigitalSignature(xmlNodePtr signatureNode);
    DigitalSignature(const DigitalSignature&) = delete;
    DigitalSignature(DigitalSignature&& o) : _signedInfo(std::move(o._signedInfo)), _keyInfo(std::move(o._keyInfo)), _object(std::move(o._object)) {}
    virtual ~DigitalSignature() {}
    
    DigitalSignature& operator=(const DigitalSignature&) = delete;
    virtual DigitalSignature& operator=(DigitalSignature&&);
    
    class SignedInfo* SignedInfo() { return _signedInfo.get(); }
    void SetSignedInfo(class SignedInfo& __i) { _signedInfo.reset(&__i); }
    const class SignedInfo* SignedInfo() const { return _signedInfo.get(); }
    
    class KeyInfo* KeyInfo() { return _keyInfo.get(); }
    void SetKeyInfo(class KeyInfo& __i) { _keyInfo.reset(&__i); }
    const class KeyInfo* KeyInfo() const { return _keyInfo.get(); }
    
    class SignatureObject* SignatureObject() { return _object.get(); }
    void SetSignatureObject(class SignatureObject& __i) { _object.reset(&__i); }
    const class SignatureObject* SignatureObject() const { return _object.get(); }
    
    bool Validate() const;
    
protected:
    Auto<class SignedInfo>        _signedInfo;
    Auto<class KeyInfo>           _keyInfo;
    Auto<class SignatureObject>   _object;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__signatures__) */
