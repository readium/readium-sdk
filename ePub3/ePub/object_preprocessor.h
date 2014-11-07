//
//  object_preprocessor.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
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
