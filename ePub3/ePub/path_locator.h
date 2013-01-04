//
//  path_locator.h
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

#ifndef __ePub3__path_locator__
#define __ePub3__path_locator__

#include "locator.h"

EPUB3_BEGIN_NAMESPACE

class PathLocator : public Locator
{
public:
    PathLocator() = delete;
    PathLocator(const std::string& path) : Locator(), _path(path) {}
    PathLocator(std::string&& path) : Locator(), _path(path) {}
    PathLocator(const PathLocator& o) : Locator(o), _path(o._path) {}
    PathLocator(PathLocator&& o) : Locator(o), _path(std::move(o._path)) {}
    
    virtual std::istream& ReadStream();
    virtual std::ostream& WriteStream();
    
    virtual Locator* dup() const { return new PathLocator(*this); }
    
    virtual bool CanReduceToPath() const { return true; }
    virtual std::string GetPath() const { return _path; }
    
    virtual std::string StringRepresentation() const { return _Str("<", _path, ">"); }
    
protected:
    const std::string _path;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__path_locator__) */
