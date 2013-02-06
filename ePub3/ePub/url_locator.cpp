//
//  url_locator.cpp
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

#include "url_locator.h"
# include "curl_streambuf.h"

EPUB3_BEGIN_NAMESPACE

URLLocator::URLLocator(const std::string& url) : Locator(), _url(url)
{
}
URLLocator::URLLocator(const IRI& url) : Locator(), _url(url)
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
        buf->open(_url.URIString().stl_str(), std::ios_base::in);
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
        buf->open(_url.URIString().stl_str(), std::ios_base::out);
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
    return _url.Path(false).stl_str();
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
    return _url.Scheme().stl_str();
}

EPUB3_END_NAMESPACE
