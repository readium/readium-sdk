//
//  filter.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-21.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __ePub3__filter__
#define __ePub3__filter__

#include <ePub3/base.h>
#include <ePub3/utilities/basic.h>
#include <ePub3/manifest.h>
#include <ePub3/encryption.h>
#include <string>
#include <functional>
#include <memory>

EPUB3_BEGIN_NAMESPACE

class Package;
class Container;

typedef std::shared_ptr<Package>    PackagePtr;

class ContentFilter;
typedef std::shared_ptr<ContentFilter>  ContentFilterPtr;

// -------------------------------------------------------------------------------------------

class ByteRange
{
public:
    ByteRange()
    {
        Reset();
    }
    
    uint32_t Location() const { return m_location; }
    void Location(uint32_t location) { m_isFullRange = false; m_location = location; }
    uint32_t Length() const { return m_length; }
    void Length(uint32_t length) { m_isFullRange = false; m_length = length; }
    bool IsFullRange() const { return m_isFullRange; }
    void Reset() { m_location = 0; m_length = 0; m_isFullRange = true; }
    
    ByteRange &operator=(const ByteRange &b)
    {
        if (b.m_isFullRange)
        {
            Reset();
        }
        else
        {
            m_location = b.m_location;
            m_length = b.m_length;
            m_isFullRange = false;
        }
        return (*this);
    }
    
private:
    ByteRange(const ByteRange &b) _DELETED_; // Delete copy constructor
    ByteRange(ByteRange &&b) _DELETED_; // Delete move constructor
    ByteRange &operator=(ByteRange &&b) _DELETED_; // Delete move assignment operator
    
    uint32_t m_location;
    uint32_t m_length;
    bool m_isFullRange;
};

// -------------------------------------------------------------------------------------------

/**
 The FilterContext abstract class can be extended by individual filters to hold
 data unique to each pass across a single stream of data.
 
 A given filter subclass can allocate and return a new FilterContext pointer by
 overriding the MakeFilterContext() function from ContentFilter. The base
 class's implementation returns `nullptr`. It is important that each invocation of
 MakeFilterContext() return a *new instance* of the appropriate object. A single
 filter instance might be used to (simultaneously) handle multiple resource streams,
 and a new context object will be requested for each new stream, to ensure state is
 kept relative to the data stream, not the filter instance.
 
 If a custom context object is created, then the filter chain will pass it to each
 invocation of FilterData() on the same object for the same data stream.
 */
class FilterContext
{
public:
    FilterContext() { }
    virtual ~FilterContext() { }
};

// -------------------------------------------------------------------------------------------

class SeekableByteStream;

/**
 The RangeFilterContext abstract class is an extension of the FilterContext class, and it is
 used to pass data needed for extracting ranges of bytes from a given resource.
 
 As you may imagine, one piece of data that we need to pass is the range of bytes that is being
 requested. That is passed by the ByteRange object that it is included in this class. In
 addition to that, the FilterContext object will need direct access to the ZIP file so that it
 can read only the pieces that it needs (after all, the whole idea was that we didn't want to
 cram a whole 1 GB video file in memory). That reference is kept the m_byteStream member
 variable.
 */

class RangeFilterContext : public FilterContext
{
public:
    RangeFilterContext() : FilterContext(), m_byteStream(nullptr) { }
    virtual ~RangeFilterContext() { }
    
    ByteRange &GetByteRange() { return m_byteRange; }
    void SetSeekableByteStream(SeekableByteStream *byteStream) { m_byteStream = byteStream; }
    SeekableByteStream *GetSeekableByteStream() const { return m_byteStream; }
    void ResetSeekableByteStream() { m_byteStream = nullptr; }
    
private:
    ByteRange m_byteRange;
    SeekableByteStream *m_byteStream;
};

// -------------------------------------------------------------------------------------------

/**
 ContentFilter is an abstract base class from which all content filters must be
 derived.
 
 It implements default handling for all the methods in its interface with
 the exception of the core data-modification method FilterData().
 
 Content filters are typically invoked with multiple chunks of data while an item
 is loaded from its container. Subclasses can override RequiresCompleteData() if
 they reqire access to all the data at once in order to function, but it is
 preferred if they support streaming, for performance reasons.
 
 Content filters are *chained*, similar to a singly-linked list. When a content
 filter is installed into a Package, it goes at the head of the list. Any existing
 filter is passed to the new one's SetNextFilter() method. In this way, multiple
 filters may be installed for a single content type, with processing proceeding in
 LIFO order.
 
 The implementation *always* queries every filter in the chain: if one filter
 modifies the data, that modified data will be seen by a later filter. This means
 that a later-added filter will not 'replace' one that was added earlier.
 
 @note The next filter is contained in a std::unique_ptr; this means each filter
 in the chain 'owns' the reference to the next, and is solely responsible for
 deleting it.
 
 @ingroup filters
 */
class ContentFilter
#if EPUB_PLATFORM(WINRT)
	: public NativeBridge
#endif
{
public:
    
    // using 'enum class' means you can ONLY use these exact values, grr
    // so I'm just making them constants
    typedef uint32_t FilterPriority;
    
    ///
    /// If you absolutely must see the bytes EXACTLY as they exist in the container, use this priority.
    static const FilterPriority MustAccessRawBytes      = 1000;
    
    ///
    /// This is the priority at which XML-ENC and XML-DSig filters take place.
    static const FilterPriority EPUBDecryption          = 750;
    
    ///
    /// This is the priority at which HTML content is modified to process `<switch>` elements and similar.
    static const FilterPriority SwitchStaticHandling    = 500;
    
    ///
    /// This is the priority at which `<object>` tags may be modified to use EPUB widgets.
    static const FilterPriority ObjectPreprocessing     = 250;
    
    ///
    /// Any items below this priority level are free to make platform-specific changes. Any validation
    /// will already have taken place by now.
    static const FilterPriority ValidationComplete      = 100;
    
    /**
     The type-sniffer function must match this prototype.
     @param item A ManifestItem to inspect. All other information can be fetched
                 through this object.
     @result Return `true` to pass this manifest item to the corresponding filter,
             `false` otherwise.
     */
    typedef std::function<bool(ConstManifestItemPtr item)> TypeSnifferFn;
    
    typedef std::function<ContentFilterPtr(ConstPackagePtr package)> TypeFactoryFn;
    
private:
    ///
    /// No default constructor.
    ContentFilter() _DELETED_;
    
public:
    ///
    /// Copy constructor.
    ContentFilter(const ContentFilter& o) : _sniffer(o._sniffer) {}
    ///
    /// C++11 move constructor.
    ContentFilter(ContentFilter&& o) : _sniffer(std::move(o._sniffer)) {}
    
    /**
	 Allocate and return a new FilterContext subclass. The default returns `nullptr`.

	 Each filter is instantiated once per Package. A filter can then be used to process
	 data from multiple ManifestItems at any one time. Any information specific to a single
	 ManifestItem can be encapsulated within a FilterContext pointer, which will be passed
	 into each invocation of the FilterData() method. The prospective ManifestItem is
	 passed into this function so that it can inform the creation of filter context data.

	 Filter context objects can be anything that inherits from ContextFilter, which itself
	 asserts no conditions on the structure or implementation of the object.
	 @param item The Manifest Item being processed, and for which the context is created.
	 @result An object containing per-item data, or nullptr.
	 */
    virtual FilterContext* MakeFilterContext(ConstManifestItemPtr item) const { return nullptr; }
    
    /**
     Create a new content filter with a (required) type sniffer.
     @param sniffer The TypeSnifferFn used to determine whether to pass certain data
     through this filter.
     */
    ContentFilter(TypeSnifferFn sniffer) : _sniffer(sniffer) {}
    virtual ~ContentFilter() {}
    
    ///
    /// Subclasses can return `true` if they need all data in one chunk.
    virtual bool RequiresCompleteData() const { return false; }
    
    /// Subclasses can return `true` if this filter supports filtering of ranges instead of full resource
    virtual bool SupportsByteRanges() const { return false; }
    
    ///
    /// Obtains the type-sniffer for this filter.
    virtual TypeSnifferFn TypeSniffer() const { return _sniffer; }
    
    ///
    /// Assigns a new type-sniffer to this filter.
    virtual void SetTypeSniffer(TypeSnifferFn fn) { _sniffer = fn; }
    
    /**
     The core processing function.
     
     The filter should apply its algorithm to the input bytes, and may opt to edit
     the data in-place (this is preferred). Any data returned from this function is
     passed to the next matching filter for processing, and will ultimately be
     returned to the user agent requesting the resource data itself.
     
     The data passed in is not guaranteed to be the entire resource unless the filter
     overrides RequiresCompleteData() to return `true`.
     @param data The data to process.
     @param len The number of bytes in `data`.
     @param outputLen Storage for the count of bytes being returned.
     @result The filtered bytes.
     @see ePub3::FontObfuscator for an example of a filter which handles data in a
     piecemeal fashion.
     @see ePub3::SwitchPreprocessor or ePub3::ObjectPreprocessor for full-data
     examples.
     */
    virtual void * FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen) = 0;
    
protected:
    TypeSnifferFn       _sniffer;
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__filter__) */
