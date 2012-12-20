//
//  path_locator.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
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
