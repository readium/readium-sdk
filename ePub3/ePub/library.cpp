//
//  library.cpp
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

#include "library.h"
#include "container.h"
#include "manifest.h"
#include "package.h"
#include "zip_archive.h"
#include <sstream>
#include <fstream>
#include <list>

// file format is CSV, unencrypted

EPUB3_BEGIN_NAMESPACE

Auto<Library> Library::_singleton(nullptr);

Library::Library(const string& path)
{
    if ( !Load(path) )
        throw std::invalid_argument("The provided Locator doesn't appear to contain library data.");
}
Library::~Library()
{
    for ( auto& item : _packages )
    {
        if ( item.second.second != nullptr )
            delete item.second.second;
    }
    for ( auto& item : _containers )
    {
        if ( item.second != nullptr )
            delete item.second;
    }
}
bool Library::Load(const string& path)
{
    std::ifstream stream(path.stl_str());
    
    std::stringstream ss;
    std::string tmp;
    while ( std::getline(stream, tmp).good() )
    {
        try
        {
            ss << tmp;
            
            string thisPath;
            std::list<std::string> uidList;
            while ( !ss.eof() )
            {
                std::getline(ss, tmp, ss.widen(','));
                if ( thisPath.empty() )
                {
                    // first item is a path to a local item
                    thisPath = tmp;
                }
                else
                {
                    // remaining items are unique IDs
                    uidList.emplace_back(tmp);
                }
            }
            
            _containers[thisPath] = nullptr;
            for ( auto uid : uidList )
            {
                _packages[uid] = {thisPath, nullptr};
            }
        }
        catch (...)
        {
            return false;
        }
    }
    
    return true;
}
Library* Library::MainLibrary(const string& path)
{
    static std::once_flag __guard;
    std::call_once(__guard, [&](){ _singleton.reset(new Library(path)); });
    return _singleton.get();
}
string Library::PathForEPubWithUniqueID(const string &uniqueID) const
{
    auto found = _packages.find(uniqueID);
    if ( found == _packages.end() )
        return string::EmptyString;
    
    return found->second.first;
}
string Library::PathForEPubWithPackageID(const string &packageID) const
{
    string uniqueIDStart(packageID + "@");
    for ( auto &pair : _packages )
    {
        if ( pair.first == packageID || pair.first.find(uniqueIDStart) == 0 )
            return pair.second.first;
    }
    
    return string::EmptyString;
}
void Library::AddPublicationsInContainer(Container* container, const string& path)
{
    // store the container
    auto existing = _containers.find(path);
    if ( existing == _containers.end() )
        _containers[path] = container;
    
    for ( auto pkg : container->Packages() )
    {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
        _packages.emplace(pkg->UniqueID(), LookupEntry({path, pkg}));
#else
        _packages[pkg->UniqueID()] = LookupEntry({path, pkg});
#endif
    }
}
void Library::AddPublicationsInContainerAtPath(const ePub3::string &path)
{
    Container *p = new Container(path);
    if ( p != nullptr )
        AddPublicationsInContainer(p, path);
}
IRI Library::EPubURLForPublication(const Package* package) const
{
    return EPubURLForPublicationID(package->UniqueID());
}
IRI Library::EPubURLForPublicationID(const string &identifier) const
{
    return IRI(IRI::gEPUBScheme, identifier, "/");
}
Package* Library::PackageForEPubURL(const IRI &url, bool allowLoad)
{
    // is it an epub URL?
    if ( url.Scheme() != IRI::gEPUBScheme )
        return nullptr;
    
    string ident = url.Host();
    auto entry = _packages.find(ident);
    if ( entry == _packages.end() )
        return nullptr;
    
    if ( entry->second.second != nullptr || !allowLoad )
        return entry->second.second;
    
    AddPublicationsInContainerAtPath(entry->second.first);
    
    // returns a package ptr or nullptr
    return entry->second.second;
}
IRI Library::EPubCFIURLForManifestItem(const ManifestItem* item) const
{
    IRI packageURL = EPubURLForPublication(item->Package());
    packageURL.SetContentFragmentIdentifier(item->Package()->CFIForManifestItem(item));
    return packageURL;
}
const ManifestItem* Library::ManifestItemForCFI(const IRI &urlWithCFI, CFI* pRemainingCFI)
{
    CFI cfi = urlWithCFI.ContentFragmentIdentifier();
    if ( cfi.Empty() )
        return nullptr;
    
    const Package* pkg = PackageForEPubURL(urlWithCFI);
    if ( pkg == nullptr )
        return nullptr;
    
    return pkg->ManifestItemForCFI(cfi, pRemainingCFI);
}
Auto<ByteStream> Library::ReadStreamForEPubURL(const IRI &url, CFI *pRemainingCFI)
{
    CFI cfi = url.ContentFragmentIdentifier();
    if ( cfi.Empty() )
    {
        // it references a content document directly
        const Package* pkg = PackageForEPubURL(url);
        if ( pkg != nullptr )
            return pkg->ReadStreamForItemAtPath(url.Path());
    }
    else
    {
        const ManifestItem* item = ManifestItemForCFI(url, pRemainingCFI);
        if ( item != nullptr )
            return item->Reader();
    }
    
    return nullptr;
}
bool Library::WriteToFile(const string& path) const
{
    std::ofstream stream(path.stl_str());
    for ( auto item : _containers )
    {
        // works like an auto_ptr, scoping the allocation
        Container* pContainer = item.second;
        
        if ( pContainer == nullptr )
            pContainer = new Container(item.first);
        if ( pContainer == nullptr )
            continue;
        
        stream << item.first;
        for ( auto pkg : pContainer->Packages() )
        {
            stream << "," << pkg->UniqueID();
        }
        
        stream << std::endl;
        if ( pContainer != item.second )
            delete pContainer;
    }
    
    return true;
}

EPUB3_END_NAMESPACE
