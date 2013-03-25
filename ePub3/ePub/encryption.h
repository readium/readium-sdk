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

#include "epub3.h"

EPUB3_BEGIN_NAMESPACE

class EncryptionInfo
{
public:
    typedef string                  algorithm_type;
    
public:
                EncryptionInfo()                            = default;
                EncryptionInfo(xmlNodePtr node);
                EncryptionInfo(const EncryptionInfo& o) : _algorithm(o._algorithm), _path(o._path) {}
                EncryptionInfo(EncryptionInfo&& o) : _algorithm(std::move(o._algorithm)), _path(std::move(o._path)) {}
    virtual     ~EncryptionInfo() {}
    
    virtual const algorithm_type&   Algorithm()                             const   { return _algorithm; }
    virtual void                    SetAlgorithm(const algorithm_type& alg)         { _algorithm = alg; }
    virtual void                    SetAlgorithm(algorithm_type&& alg)              { _algorithm = alg; }
    
    virtual const string&           Path()                                  const   { return _path; }
    virtual void                    SetPath(const string& path)                     { _path = path; }
    virtual void                    SetPath(string&& path)                          { _path = path; }
    
protected:
    algorithm_type  _algorithm;
    string          _path;

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__encryption__) */
