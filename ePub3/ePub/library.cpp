//
//  library.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "library.h"
#include "container.h"
#include "url_locator.h"
#include "path_locator.h"
#include <sstream>
#include <list>

// file format is CSV, unencrypted

EPUB3_BEGIN_NAMESPACE

Shared<Library> Library::_singleton = Shared<Library>();

Library::Library(Locator* locator)
{
    if ( !Load(locator) )
        throw std::invalid_argument("The provided Locator doesn't appear to contain library data.");
}
bool Library::Load(Locator* locator)
{
    std::istream& stream = locator->ReadStream();
    
    std::stringstream ss;
    std::string tmp;
    while ( std::getline(stream, tmp).good() )
    {
        try
        {
            ss << tmp;
            
            Shared<Locator> thisLoc;
            std::list<std::string> uidList;
            while ( !ss.eof() )
            {
                std::getline(ss, tmp, ss.widen(','));
                if ( thisLoc.get() == nullptr )
                {
                    // first item is a path to a local item
                    thisLoc.reset(new PathLocator(tmp));
                }
                else
                {
                    uidList.emplace_back(tmp);
                }
            }
            
            _containers[thisLoc] = nullptr;
            for ( auto uid : uidList )
            {
                _packages[uid] = {thisLoc, nullptr};
            }
        }
        catch (...)
        {
            return false;
        }
    }
    
    return true;
}
Shared<Library> Library::MainLibrary(Locator* locator)
{
    if ( (bool)_singleton )
        return _singleton;
    
    _singleton.reset(new Library(locator));
    return _singleton;
}
Locator Library::LocatorForEPubWithUniqueID(const std::string &uniqueID) const
{
    auto found = _packages.find(uniqueID);
    if ( found == _packages.end() )
        return NullLocator();
    
    return *(found->second.first);
}
void Library::AddEPubsInContainer(Shared<Container> container, Locator* locator)
{
    Shared<Locator> ptr(locator);
    
    // store the container
    auto existing = _containers.find(ptr);
    if ( existing == _containers.end() )
        _containers[ptr] = container;
    
    for ( auto pkg : container->Packages() )
    {
        _packages[pkg->UniqueID()] = {ptr, Shared<Package>(pkg)};
    }
}
std::string Library::EPubURLForPackage(Shared<const Package> package) const
{
    return EPubURLForPackage(package->UniqueID());
}
std::string Library::EPubURLForPackage(const std::string &identifier) const
{
    return _Str("epub3://", identifier, "/");
}
Package* Library::PackageForEPubURL(const std::string &url)
{
    // get the uid
    auto loc = url.find("epub3://");
    if ( loc != 0 )
        return nullptr;
    
    loc = url.find_first_of("/", 8, url.size()-8);
    if ( loc == std::string::npos )
        return nullptr;
    
    std::string ident(url.substr(loc, url.size()-loc-1));
    auto entry = _packages.find(ident);
    if ( entry == _packages.end() )
        return nullptr;
    
    if ( entry->second.second.get() != nullptr )
        return entry->second.second.get();
    
    AddEPubsInContainerAtPath(entry->second.first.get());
    
    // returns a package ptr or nullptr
    return entry->second.second.get();
}
std::string Library::EPubCFIURLForManifestItem(Shared<const ManifestItem> item)
{
    return _Str(EPubURLForPackage(item->Package()), item->Package()->CFISubpathForManifestItemWithID(item->Identifier()));
}
bool Library::WriteToFile(Locator* locator) const
{
    std::ostream& stream = locator->WriteStream();
    for ( auto item : _containers )
    {
        // works like an auto_ptr, scoping the allocation
        Shared<Container> pContainer = item.second;
        
        if ( pContainer == nullptr )
            pContainer.reset(new Container(*item.first));
        
        stream << item.first->GetPath();
        for ( auto pkg : pContainer->Packages() )
        {
            stream << "," << pkg->UniqueID();
        }
        
        stream << std::endl;
    }
    
    return true;
}

EPUB3_END_NAMESPACE
