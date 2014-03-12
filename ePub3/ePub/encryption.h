//
//  encryption.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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
                    EncryptionInfo(ContainerPtr owner) : OwnedBy(owner), _algorithm(), _path() {}
    ///
    /// Copy constructor.
                    EncryptionInfo(const EncryptionInfo& o) : OwnedBy(o), _algorithm(o._algorithm), _path(o._path) {}
    ///
    /// Move constructor.
                    EncryptionInfo(EncryptionInfo&& o) : OwnedBy(std::move(o)), _algorithm(std::move(o._algorithm)), _path(std::move(o._path)) {}
    virtual         ~EncryptionInfo() {}
    
    
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
    /// Returns the Container-relative path to the encrypted resource.
    virtual const string&           Path()                                  const   { return _path; }
    ///
    /// Assigns a Container-relative path to an encrypted resource.
    virtual void                    SetPath(const string& path)                     { _path = path; }
    virtual void                    SetPath(string&& path)                          { _path = path; }
    
protected:
    algorithm_type  _algorithm;     ///< The algorithm identifier, as per XML-ENC or OCF.
    string          _path;          ///< The Container-relative path to an encrypted resource.

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__encryption__) */
