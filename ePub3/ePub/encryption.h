//
//  encryption.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
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

#ifndef __ePub3__encryption__
#define __ePub3__encryption__

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>

EPUB3_BEGIN_NAMESPACE

class Container;

/**
 Contains details on the encryption of a single resource.
 
 At present this class holds only two pieces of information: A resource path and an
 encyption algorithm identifier (a URI string). This is all that's needed to support
 EPUB font obfuscation.
 
 In future, this class will grow to encapsulate all information from the XML-ENC
 specification.
 
 @ingroup epub-model
 @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#font-obfuscation
 @see http://www.w3.org/TR/xmlenc-core1/
 */
class EncryptionInfo : public PointerType<EncryptionInfo>, public OwnedBy<Container>
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
{
public:
    ///
    /// Encryption algorithms are URIs compared as strings.
    typedef string                  algorithm_type;
    
public:
    ///
    /// Creates a new EncryptionInfo with no details filled in.
    EncryptionInfo(ContainerPtr owner) : OwnedBy(owner), _algorithm(), _path(), _compression_method(), _uncompressed_size(), _keyRetrievalMethodType() {}
    ///
    /// Copy constructor.
    EncryptionInfo(const EncryptionInfo& o) : OwnedBy(o), _algorithm(o._algorithm), _path(o._path), _compression_method(o._compression_method), _uncompressed_size(o._uncompressed_size), _keyRetrievalMethodType(o._keyRetrievalMethodType) {}
    ///
    /// Move constructor.
    EncryptionInfo(EncryptionInfo&& o) : OwnedBy(std::move(o)), _algorithm(std::move(o._algorithm)), _path(std::move(o._path)),  _compression_method(std::move(o._compression_method)), _uncompressed_size(std::move(o._uncompressed_size)), _keyRetrievalMethodType(std::move(o._keyRetrievalMethodType)) {}
    virtual ~EncryptionInfo() {}
    
    
    /**
     Creates a new EncryptionInfo from an EncryptionData XML element node.
     @param node An XML node, which *must* be an `EncryptionData` node as defined in
     XML-ENC.
     @see http://www.w3.org/TR/xmlenc-core1/#sec-EncryptedData
     */
    EPUB3_EXPORT
    bool            ParseXML(shared_ptr<xml::Node> node);
    
    ///
    /// Returns an algorithm URI as defined in XML-ENC or OCF.
    /// @see http://www.w3.org/TR/xmlenc-core1/#sec-Table-of-Algorithms
    /// @see http://www.idpf.org/epub/30/spec/epub30-ocf.html#fobfus-specifying
    virtual const algorithm_type&   Algorithm()                             const   { return _algorithm; }
    ///
    /// Assigns an algorithm identifier.
    /// @see Algorithm()const
    virtual void                    SetAlgorithm(const algorithm_type& alg)         { _algorithm = alg; }
    virtual void                    SetAlgorithm(algorithm_type&& alg)              { _algorithm = alg; }
    
    ///
    /// Returns the type of the ds:RetrievalMethod of an Encrypted Key as an URI.
    /// @see http://www.w3.org/TR/2001/WD-xmlenc-core-20010626/#sec-ds:RetrievalMethod
    virtual const string&           KeyRetrievalMethodType()                const   { return _keyRetrievalMethodType; }
    ///
    /// Assigns the URI for the ds:RetrievalMethod type.
    virtual void                    SetKeyRetrievalMethodType(const string& keyRetrievalMethodType) { _keyRetrievalMethodType = keyRetrievalMethodType; }
    virtual void                    SetKeyRetrievalMethodType(string&& keyRetrievalMethodType)      { _keyRetrievalMethodType = keyRetrievalMethodType; }

    ///
    /// Returns the Container-relative path to the encrypted resource.
    virtual const string&           Path()                                  const   { return _path; }
    ///
    /// Assigns a Container-relative path to an encrypted resource.
    virtual void                    SetPath(const string& path)                     { _path = path; }
    virtual void                    SetPath(string&& path)                          { _path = path; }
    
    // Added by DRM inside T.H. Kim on 2015-04-15
    // Return additional information for the compressed and encrypted contents
    virtual const string&           CompressionMethod()                     const   { return _compression_method; }
    virtual const string&           UnCompressedSize()                      const   { return _uncompressed_size;}


protected:
    algorithm_type  _algorithm;              ///< The algorithm identifier, as per XML-ENC or OCF.
    string          _keyRetrievalMethodType; ///< The method used to retrieve the encryption key (eg. used for LCP)
    string          _path;                   ///< The Container-relative path to an encrypted resource.

    // Added by DRM inside T.H. Kim on 2015-04-15
    // To get additional information for the compressed and encrypted contents
    string          _compression_method;  //  Compression method : 0(no compression), 8(deflated)
    string          _uncompressed_size;   //  Uncompressed size of the content

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__encryption__) */
