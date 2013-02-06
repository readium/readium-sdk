//
//  library.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012-2013 The Readium Foundation.
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
#include "url_locator.h"
#include "path_locator.h"
#include <sstream>
#include <list>

// file format is CSV, unencrypted

EPUB3_BEGIN_NAMESPACE

Auto<Library> Library::_singleton;

Library::Library(Locator* locator)
{
    if ( !Load(locator) )
        throw std::invalid_argument("The provided Locator doesn't appear to contain library data.");
}
Library::~Library()
{
    _packages.clear();
    for ( auto item : _containers )
    {
        delete item.first;
        delete item.second;
    }
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
            
            Locator* thisLoc = nullptr;
            std::list<std::string> uidList;
            while ( !ss.eof() )
            {
                std::getline(ss, tmp, ss.widen(','));
                if ( thisLoc == nullptr )
                {
                    // first item is a path to a local item
                    thisLoc = new PathLocator(tmp);
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
Library* Library::MainLibrary(Locator* locator)
{
    if ( (bool)_singleton )
        return _singleton.get();
    
    _singleton.reset(new Library(locator));
    return _singleton.get();
}
Locator Library::LocatorForEPubWithUniqueID(const string& uniqueID) const
{
    auto found = _packages.find(uniqueID);
    if ( found == _packages.end() )
        return NullLocator();
    
    return *(found->second.first);
}
void Library::AddEPubsInContainer(Container* container, Locator* locator)
{
    // store the container
    auto existing = _containers.find(locator);
    if ( existing == _containers.end() )
        _containers[locator] = container;
    
    for ( auto pkg : container->Packages() )
    {
        _packages.emplace(pkg->UniqueID(), LookupEntry({locator, pkg}));
    }
}
string Library::EPubURLForPackage(const Package* package) const
{
    return EPubURLForPackage(package->UniqueID());
}
string Library::EPubURLForPackage(const string &identifier) const
{
    return _Str("epub3://", identifier, "/");
}
Package* Library::PackageForEPubURL(const string &url)
{
    // get the uid
    auto loc = url.find("epub3://");
    if ( loc != 0 )
        return nullptr;
    
    loc = url.find_first_of("/", 8, url.size()-8);
    if ( loc == std::string::npos )
        return nullptr;
    
    string ident(url.substr(loc, url.size()-loc-1));
    auto entry = _packages.find(ident);
    if ( entry == _packages.end() )
        return nullptr;
    
    if ( entry->second.second != nullptr )
        return entry->second.second;
    
    AddEPubsInContainerAtPath(entry->second.first);
    
    // returns a package ptr or nullptr
    return entry->second.second;
}
string Library::EPubCFIURLForManifestItem(const ManifestItem* item)
{
    return _Str(EPubURLForPackage(item->Package()), item->Package()->CFISubpathForManifestItemWithID(item->Identifier()));
}
bool Library::WriteToFile(Locator* locator) const
{
    std::ostream& stream = locator->WriteStream();
    for ( auto item : _containers )
    {
        // works like an auto_ptr, scoping the allocation
        Container* pContainer = item.second;
        
        if ( pContainer == nullptr )
            pContainer = new Container(*item.first);
        
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
