//
//  library.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
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

#ifndef __ePub3__library__
#define __ePub3__library__

#include <ePub3/epub3.h>
#include <ePub3/container.h>
#include <ePub3/package.h>
#include <ePub3/cfi.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/byte_stream.h>
#include <map>

EPUB3_BEGIN_NAMESPACE

// Note that this is a library in the smallest sense: it keeps track of ePub files
//  by their unique-identifier, storing the path to that ePub file *as given*. Its
//  primary presence here is to allow for inter-publication linking. It is also
//  designed to be subclassed in order to serve as the basis of a complete library
//  in an application, with all the additional functionality that would involve.
//
// Note that each application would have a single library, accessible only through
//  the MainLibrary() static method. A library can optionally be written out to
//  disk and then loaded back in by calling MainLibrary() with a Locator instance
//  at application startup. Once the singleton instance has been created,
//  MainLibrary() will ignore its argument and always return that instance.
//
// Thoughts: OCF allows for multiple packages to be specified, but I don't see any
//  handling of that in ePub3 CFI?

/**
 @ingroup utilities
 */
class Library
{
public:
    typedef string      EPubIdentifier;
    
protected:
                        Library() : _containers(), _packages() {}
                        Library(const Library& o) : _containers(o._containers), _packages(o._packages) {}
                        Library(Library&& o) : _containers(std::move(o._containers)), _packages(std::move(o._packages)) {}
    
    // load a library from a file generated using WriteToFile()
                        Library(const string& path);
    bool                Load(const string& path);
    
public:
    // access a singleton instance managed by the class
    static Library*     MainLibrary(const string& path = "");
    virtual             ~Library();
    
    string              PathForEPubWithUniqueID(const string& uniqueID)     const;
    string              PathForEPubWithPackageID(const string& packageID)   const;
    
    void                AddPublicationsInContainer(Container* container, const string& path);
    void                AddPublicationsInContainerAtPath(const string& path);
    
    // returns an epub3:// url for the package with a given identifier
    IRI                 EPubURLForPublication(const Package* package)       const;
    IRI                 EPubURLForPublicationID(const string& identifier)   const;
    
    // may load a container/package, so non-const
    Package*            PackageForEPubURL(const IRI& url, bool allowLoad=true);
    
    IRI                 EPubCFIURLForManifestItem(const ManifestItem* item) const;
    
    // may instantiate a Container & store it, so non-const
    const ManifestItem* ManifestItemForCFI(const IRI& urlWithCFI, CFI* pRemainingCFI);
    
    Auto<ByteStream>    ReadStreamForEPubURL(const IRI& url, CFI* pRemainingCFI);
    
    // file format is sort-of CSV
    // each line starts with a container locator's string representation followed by a
    //  comma-separated list of package identifiers
    bool                WriteToFile(const string& path)                     const;
    
protected:
    // list of known (but not necessarily loaded) containers
    typedef std::map<string, Container*>            ContainerLookup;
    
    // if container is loaded, LookupEntry will contain a Package
    // otherwise, the locator is used to load the Container
    typedef std::pair<string, Package*>             LookupEntry;
    typedef std::map<EPubIdentifier, LookupEntry>   PackageLookup;
    
    ContainerLookup         _containers;
    PackageLookup           _packages;
    
    static Auto<Library>    _singleton;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__library__) */
