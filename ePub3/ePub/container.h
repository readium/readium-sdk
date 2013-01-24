//
//  container.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#ifndef __ePub3__container__
#define __ePub3__container__

#include "epub3.h"
#include "locator.h"
#include "encryption.h"
#include "package.h"
#include "utfstring.h"
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <vector>

EPUB3_BEGIN_NAMESPACE

class Archive;

class Container
{
public:
    typedef std::vector<string>             PathList;
    typedef std::vector<Package*>           PackageList;
    typedef std::vector<EncryptionInfo*>    EncryptionList;
    
public:
                Container(const std::string& path);
                Container(Locator locator);
                Container(const Container&)                 = delete;
                Container(Container&& o);
    virtual     ~Container();
    
    virtual PathList                PackageLocations()      const;
    virtual const PackageList&      Packages()              const   { return _packages; }
    virtual string                  Version()               const;
    virtual const EncryptionList&   EncryptionData()        const   { return _encryption; }
    
    virtual const EncryptionInfo*   EncryptionInfoForPath(const string& path)  const;
    
protected:
    Archive *       _archive;
    xmlDocPtr       _ocf;
    PackageList     _packages;
    EncryptionList  _encryption;
    
    void        LoadEncryption();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__container__) */
