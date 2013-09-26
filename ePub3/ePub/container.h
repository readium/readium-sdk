//
//  container.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#ifndef __ePub3__container__
#define __ePub3__container__

#include <ePub3/epub3.h>
#include <ePub3/encryption.h>
#include <ePub3/package.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/xml/node.h>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class Archive;
class ByteStream;

/**
 The Container class provides an interface for interacting with an EPUB container,
 i.e. a `.epub` file.
 
 Each Container instance owns all its sub-items. All Packages from a single container,
 for instance, are kept around as pointers which will be deleted when the container
 is destroyed.
 
 @remarks The Container class holds owning references to the Archive instance used
 to read from the zip file, the XML document for the OCF file at META-INF/container.xml,
 all Packages within the container, and all EncryptionInfo instances from
 META-INF/encryption.xml.
 
 @ingroup epub-model
 */
class Container : public PointerType<Container>
{
public:
    ///
    /// A list of container sub-item paths.
    typedef std::vector<string>                 PathList;
    ///
    /// A list of Packages.
    typedef shared_vector<Package>              PackageList;
    ///
    /// A list of encryption information.
    typedef shared_vector<EncryptionInfo>       EncryptionList;

private:
    ///
    /// There is no copy constructor.
                    Container(const Container&)                 _DELETED_;
    
protected:
    ///
    /// C++11 move constructor.
    EPUB3_EXPORT    Container(Container&& o);
    
public:
    /**
     Create a new Container.
     */
    EPUB3_EXPORT    Container();
    
    ///
    /// Opens the archive at a given path.
    bool            Open(const string& path);
    
    ///
    /// Creates and returns a new Container instance.
    static ContainerPtr    OpenContainer(const string& path);
    
    virtual         ~Container();
    
    ///
    /// Retrieves the paths for all Package documents in the container.
    virtual PathList                PackageLocations()      const;
    
    ///
    /// Retrieves the list of all instantiated packages within the container.
    virtual const PackageList&      Packages()              const   { return _packages; }
    
    /**pack
     Retrieves the default Package instance.
     
     Equivalent to `this->Packages().at(0)`.
     */
    virtual PackagePtr              DefaultPackage()        const;
    
    ///
    /// The OCF version of the container document.
    virtual string                  Version()               const;
    
    ///
    /// Retrieves the encryption information embedded in the container.
    virtual const EncryptionList&   EncryptionData()        const   { return _encryption; }
    
    /**
     Retrieves the encryption information for a specific file within the container.
     @param path A container-relative path to the item whose encryption information
     to retrieve.
     @result Returns the encryption information, or `nullptr` if none was found.
     */
    virtual EncryptionInfoPtr       EncryptionInfoForPath(const string& path)   const;
    
    /**
     Obtains a pointer to a ReadStream for a specific file within the container.
     @param path A container-relative path to the file whose data to read.
     @result A std::unique_ptr for a new stream to the specified file, or `nullptr`
     if the file was not found.
     */
    virtual unique_ptr<ByteStream>  ReadStreamAtPath(const string& path)        const;
    
    ///
    /// The underlying archive.
    ArchivePtr                      GetArchive()            const   { return _archive; }
    
    
protected:
    ArchivePtr						_archive;
    shared_ptr<xml::Document>		_ocf;
    PackageList						_packages;
    EncryptionList					_encryption;
    
    ///
    /// Parses the file META-INF/encryption.xml into an EncryptionList.
    void							LoadEncryption();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__container__) */
