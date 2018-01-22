//
//  object_preprocessor.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __ePub3__object_preprocessor__
#define __ePub3__object_preprocessor__

#include <ePub3/epub3.h>
#include <ePub3/filter.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/content_handler.h>
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

class Package;

/**
 Implements a filter for reading content documents which statically replaces `object`
 elements with `iframe` elements referencing the appropriate DHTML handler.
 @ingroup filters
 */
class ObjectPreprocessor : public ContentFilter, public PointerType<ObjectPreprocessor>
{
protected:
    ///
    /// Matches only mnifest items with a media-type of "application/xhtml+xml" or "text/html".
    static bool ShouldApply(ConstManifestItemPtr item);
    
    /// The factory routine
    static ContentFilterPtr ObjectFilterFactory(ConstPackagePtr package);

private:
    ///
    /// No default constructor.
    ObjectPreprocessor()                                    _DELETED_;
    
public:
    /**
     Initializes a preprocessor and associates it with a Package object, from which
     it can obtain foreign media handler details.
     @param pkg The package to which this filter will apply.
     */
    EPUB3_EXPORT
    ObjectPreprocessor(ConstPackagePtr pkg, const string& openButtonTitle = "Open Fullscreen");
    
    ///
    /// Standard copy constructor.
    ObjectPreprocessor(const ObjectPreprocessor& o) : ContentFilter(o), _objectMatcher(o._objectMatcher), _handlers(o._handlers) {}
    
    ///
    /// C++11 'move' constructor.
    ObjectPreprocessor(ObjectPreprocessor&& o) : ContentFilter(std::move(o)), _objectMatcher(std::move(o._objectMatcher)), _handlers(std::move(o._handlers)) {}
    
    ///
    /// Destructor.
    virtual ~ObjectPreprocessor() {}
    
    ///
    /// This preprocessor requires access to the entire content document at once.
    virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::RequiresCompleteData; }
    
    /**
     Performs the static replacement of `object` tags whose `type` attribute
     identifies a media-type for which the Publication provides a media handler.
     
     The entire `object` element will be replaced wholesale with two elements: an
     `iframe` containing the handler and a `form` containing a `button` element
     which will open the handler full-screen.  The `iframe` will be sandboxed, and
     will look similar to the following:
     
         <iframe src="src.xml" srcdoc="src.xml" id="some_id"
                 sandbox="allow-forms allow-scripts allow-same-origin"
                 seamless="seamless">
         </iframe>
     
     Note that the XHTML vocabulary of HTML5 is used, and that both a `srcdoc`
     attribute for HTML5 and a `src` for older renderers is included.
     
     If the source `object` tag has an `id` attribute, that id will be attached to
     the new `iframe` element.  Additionally, the id will be suffixed with `-form`
     and `-button` and applied to the `form` and `button` elements respectively.  It
     is our intention that these rules will make it possible for content authors to
     anticipate these substitutions and build CSS or JavaScript rules directly.
     */
    virtual void*   FilterData(FilterContext* context, void* data, size_t len, size_t* outputLen) OVERRIDE;
    
    // register with the filter manager
    static void Register();
    
protected:
    /**
     This regex is used to locate only those `object` tags for which handlers exist.
     
     Its text is:
     
         <object\s+?([^>]*?media-type="(...|...|...)"[^>]*?)>(.*?)</object>
     
     The three ellipses in the middle represent media types. For example, with
     handlers for 'application/x-slideshow+xml' and 'image/x-slideshow', the regex
     would read:
     
         <object\s+?([^>]*?type="(application/x-slideshow\.xml|image/x-slideshow)"[^>]*?)>(.*?)</object>
     
     Note that any and all regex special characters will be replaced automatically
     during insertion.
     
     #### Matches
     
     There are a few things captured by the expression:
     
     - *0:* The entire `<object...>...</object>` construct.
     - *1:* The complete string containing all the attribute definitions of the `<object>` tag.
     - *2:* The value of the `object` element's `type` attribute.
     - *3:* The `innerHTML` content of the `object` element.
     */
    REGEX_NS::regex                         _objectMatcher;
    
    ///
    /// The (hopefully localized!) title of the generated HTML5 `<button>`.
    const string                            _button;
    
    ///
    /// The object keeps its own list of handlers, used to create target URIs.
    std::map<string, MediaHandler>          _handlers;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__object_preprocessor__) */
