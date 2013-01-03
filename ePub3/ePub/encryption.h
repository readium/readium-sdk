//
//  encryption.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-28.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__encryption__
#define __ePub3__encryption__

#include "epub3.h"

EPUB3_BEGIN_NAMESPACE

class EncryptionInfo
{
public:
    typedef std::string         algorithm_type;
    
public:
    EncryptionInfo() = default;
    EncryptionInfo(xmlNodePtr node);
    EncryptionInfo(const EncryptionInfo& o) : _algorithm(o._algorithm), _path(o._path) {}
    EncryptionInfo(EncryptionInfo&& o) : _algorithm(std::move(o._algorithm)), _path(std::move(o._path)) {}
    virtual ~EncryptionInfo() {}
    
    virtual const algorithm_type& Algorithm() const { return _algorithm; }
    virtual void SetAlgorithm(const algorithm_type& alg) { _algorithm = alg; }
    virtual void SetAlgorithm(algorithm_type&& alg) { _algorithm = alg; }
    
    virtual const std::string& Path() const { return _path; }
    virtual void SetPath(const std::string& path) { _path = path; }
    virtual void SetPath(std::string&& path) { _path = path; }
    
protected:
    algorithm_type  _algorithm;
    std::string     _path;

};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__encryption__) */
