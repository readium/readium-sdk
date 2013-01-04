//
//  locator.h
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

#ifndef __ePub3__locator__
#define __ePub3__locator__

#include "epub3.h"

EPUB3_BEGIN_NAMESPACE

// Welcome to our first Expandability and Pluggability Showcase!
//
// The class here serves as an abstract base class for a 'locator' type. The idea
//  is that there will be concrete subclasses created on different platforms to
//  provide support for different file-location schemes. Examples (concrete, even!)
//  include path strings, URLs (concrete example uses libcurl), and Apple's
//  'Bookmark Data' or File-ID URLs (concrete example uses CFURLRef).
//
// Each locator should be able to provide an istream/ostream to the resource to
//  which it refers. These could be swapped for our own stream classes, naturally,
//  but for the present I'm sticking with C++11's iostream classes as the base.
//
// NB: since the copy/move constructors for C++ iostreams are deleted, we can't
//  return a new stream each time ReadStream() or WriteStream() are called. As a
//  result, we keep these streams around internally and return them directly as
//  references. That means a couple of things:
//
//  1. A single Locator instance being used by multiple threads is a Bad Idea.
//  2. The copy/move constructors for Locator explicitly ignore the streams, so
//     copies have to create their own.
//  3. Always pass a Locator around BY VALUE unless you really know what you're
//     doing.
//  4. There has to be a better way...

class Locator
{
public:
    Locator() = default;
    Locator(const Locator&) {}
    Locator(Locator&&) {}
    virtual ~Locator();     // deletes the stream members-- subclasses don't have to
    
    virtual std::istream& ReadStream() = 0;
    virtual std::ostream& WriteStream() = 0;
    
    virtual Locator* dup() const = 0;
    
    // Some APIs actually need a *path*
    // Give them a way of getting one if possible
    virtual bool CanReduceToPath() const { return false; }
    virtual std::string GetPath() const { return ""; }
    
    // A string representation is useful
    virtual std::string StringRepresentation() const = 0;
    
protected:
    std::istream *  _reader;
    std::ostream *  _writer;
};

class NullLocator : public Locator
{
public:
    NullLocator();
    NullLocator(const NullLocator& o) : Locator(o) {}
    NullLocator(NullLocator&& o) : Locator(o) {}
    virtual ~NullLocator() {}
    
    virtual Locator* dup() const { return new NullLocator(*this); }
    
    // returns null streams which do nothing
    virtual std::istream& ReadStream() { return *_reader; }
    virtual std::ostream& WriteStream() { return *_writer; }
    
    virtual std::string StringRepresentation() const { return "<null>"; }
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__locator__) */
