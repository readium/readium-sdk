//
//  url_locator.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-14.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "url_locator.h"
#include "curl_streambuf.h"

EPUB3_BEGIN_NAMESPACE

URLLocator::URLLocator(const std::string& url) : Locator(), _url(url)
{
}
URLLocator::URLLocator(const URLLocator& o) : Locator(o), _url(o._url)
{
}
URLLocator::URLLocator(URLLocator&& o) : Locator(o), _url(std::move(o._url))
{
}
URLLocator::~URLLocator()
{
}
std::istream& URLLocator::ReadStream()
{
    if ( _reader == nullptr )
    {
        curlbuf* buf = new curlbuf;
        buf->open(_url, std::ios_base::in);
        _inbuf.reset(buf);
        _reader = new std::istream(buf);
    }
    return *_reader;
}
std::ostream& URLLocator::WriteStream()
{
    if ( _writer == nullptr )
    {
        curlbuf *buf = new curlbuf;
        buf->open(_url, std::ios_base::out);
        _outbuf.reset(buf);
        _writer = new std::ostream(buf);
    }
    return *_writer;
}
bool URLLocator::CanReduceToPath() const
{
    return scheme() == "file";
}
std::string URLLocator::GetPath() const
{
    if ( !CanReduceToPath() )
        return "";
    
    // we *know* it starts with 'file://' now
    auto loc = _url.find_first_of('/', 7);
    if ( loc == std::string::npos )
        return "";
    
    // either 'file://localhost/something' or 'file:///something'
    // in first case, loc is 16, in second 7, both yield '/something'
    return _url.substr(loc);
}
template <typename... Args>
bool URLLocator::SetOption(CURLoption opt, const Args&... args)
{
    if ( (bool)_inbuf )
        dynamic_cast<curlbuf*>(_inbuf.get())->SetOption(opt, args...);
    if ( (bool)_outbuf )
        dynamic_cast<curlbuf*>(_outbuf.get())->SetOption(opt, args...);
}
bool URLLocator::SupportsURLScheme(const std::string& url)
{
    return curlbuf::CanHandleURLScheme(url);
}
std::string URLLocator::scheme() const
{
    auto loc = _url.find_first_of(':');
    if ( loc == std::string::npos )
        return "";
    return _url.substr(0, loc);
}

EPUB3_END_NAMESPACE
