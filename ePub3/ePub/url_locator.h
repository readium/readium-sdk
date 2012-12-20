//
//  url_locator.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__url_locator__
#define __ePub3__url_locator__

#include "locator.h"
#include <curl/curl.h>
#include <map>

EPUB3_BEGIN_NAMESPACE

class URLLocator : public Locator
{
public:
    URLLocator() = delete;
    URLLocator(const std::string& url);
    URLLocator(const URLLocator& o);
    URLLocator(URLLocator&& o);
    virtual ~URLLocator();
    
    virtual std::istream& ReadStream();
    virtual std::ostream& WriteStream();
    
    virtual Locator* dup() const { return new URLLocator(*this); }
    
    virtual bool CanReduceToPath() const;
    virtual std::string GetPath() const;
    
    virtual std::string StringRepresentation() const { return _Str("<", _url, ">"); }
    
    template <typename... Args>
    bool SetOption(CURLoption opt, const Args&... args);
    
    static bool SupportsURLScheme(const std::string& url);
    
protected:
    std::string                     _url;
    std::shared_ptr<std::streambuf> _inbuf;
    std::shared_ptr<std::streambuf> _outbuf;
    
    std::string scheme() const;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__url_locator__) */
