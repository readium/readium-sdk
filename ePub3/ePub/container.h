//
//  container.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#ifndef __ePub3__container__
#define __ePub3__container__

#include <ePub3/epub3.h>
#include <ePub3/encryption.h>
#include <ePub3/package.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/content_module.h>
#include <ePub3/xml/node.h>
#include <vector>
#include <ePub3/utilities/future.h>

///////////////////////////////////////////////////////////////////////////////////
// Bit of a hack -- make the WinRT Container class available so we can befriend it.

#if EPUB_PLATFORM(WINRT)
namespace Readium
{
	ref class Container;
}
#endif

// End hack
///////////////////////////////////////////////////////////////////////////////////

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
#if EPUB_PLATFORM(WINRT)
	, public NativeBridge
#endif
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
    bool            Open(const string& path, bool skipLoadingPotentiallyEncryptedContent = false);
    
    ///
    /// Creates and returns a new Container instance by calling OpenContainerAsync() and blocking.
    static ContainerPtr
        OpenContainer(const string& path);

#ifdef SUPPORT_ASYNC
    ///
    /// Asynchronously returns a new Container instance.
    static future<ContainerPtr>
        OpenContainerAsync(const string& path, launch policy = launch::any);
#endif /* SUPPORT_ASYNC */

	///
	/// Synchronously creates a new container. Available for the use of ContentModule implementations only.
	static ContainerPtr
		OpenContainerForContentModule(const string& path, bool skipLoadingPotentiallyEncryptedContent = false); 
    
    virtual         ~Container();
    
    ///
    /// Retrieves the paths for all Package documents in the container.
    virtual PathList                PackageLocations()      const;
    
    ///
    /// Retrieves the list of all instantiated packages within the container.
    virtual const PackageList&      Packages()              const   { return _packages; }
    
    /**
     Retrieves the default Package instance.
     
     Equivalent to `this->Packages().at(0)`.
     */
    virtual PackagePtr              DefaultPackage()        const;
    
    ///
    /// The OCF version of the container document.
    virtual string                  Version()               const;

	const string&                   Path()                  const   { return _path; }
    
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
	 Determines whether a given file is present in the container.
	 @param path The absolute path of the item.
	 @result `true` if the item exists, `false` otherwise.
	 */
	virtual bool					FileExistsAtPath(const string& path)		const;
    
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

	///
	/// Returns the ContentModule which created this container, if any.
	std::shared_ptr<ContentModule>	Creator()				const	{ return _creator; }

	///
	/// Asserts ownership of a Container from a ContentModule.
	void							SetCreator(std::shared_ptr<ContentModule> creator)
	{
		if (bool(_creator))
			throw std::runtime_error("Attempt to set a second Creator on a Container instance");
		_creator = creator;
	}

protected:
    ///
    /// Check for vendor-specific metadata
    /// http://www.idpf.org/epub/fxl/#mappings
    /// TODO: currently only checks /META-INF/com.apple.ibooks.display-options.xml
    /// (+ Sony, Amazon, etc.?)
    void ParseVendorMetadata();
    string _appleIBooksDisplayOption_FixedLayout; // true | false
    string _appleIBooksDisplayOption_Orientation; // landscape-only | portrait-only | none
public:
    // TODO: this API does not need to be public, only used internally by Package::Open() to adjust OPF rendition:layout|orientation properties
    const string GetVendorMetadata_AppleIBooksDisplayOption_FixedLayout() const { return _appleIBooksDisplayOption_FixedLayout; };
    const string GetVendorMetadata_AppleIBooksDisplayOption_Orientation() const { return _appleIBooksDisplayOption_Orientation; };

protected:
    ArchivePtr						_archive;
    shared_ptr<xml::Document>		_ocf;
    PackageList						_packages;
    EncryptionList					_encryption;
	std::shared_ptr<ContentModule>	_creator;
	string							_path;
    
    ///
    /// Parses the file META-INF/encryption.xml into an EncryptionList.
    void							LoadEncryption();

	//////////////////////////////////////////////////////////////////////////////
	// BLATANT HACK!
	//
	// This is here because we're seeing weird stuff happen with nested IAsyncAction()
	// stuff on WinRT, and we've got 2 days to make it work. Proper solution forthcoming.

#if EPUB_PLATFORM(WINRT)
	friend ref class ::Readium::Container;
	static ContainerPtr OpenSynchronouslyForWinRT(const string& path);
#endif

	// End hack
	//////////////////////////////////////////////////////////////////////////////

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__container__) */
