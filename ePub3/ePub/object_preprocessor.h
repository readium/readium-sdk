//
//  object_preprocessor.h
//  ePub3
//
//  Created by Jim Dovey on 2013-01-31.
//  Copyright (c) 2013 The Readium Foundation. All rights reserved.
//

#ifndef __ePub3__object_preprocessor__
#define __ePub3__object_preprocessor__

#include "epub3.h"
#include "filter.h"
#include "iri.h"
#include "content_handler.h"
#include <regex>

EPUB3_BEGIN_NAMESPACE

class Package;

/**
 Implements a filter for reading content documents which statically replaces `object`
 elements with `iframe` elements referencing the appropriate DHTML handler.
 */
class ObjectPreprocessor : public ContentFilter
{
protected:
    ///
    /// Matches only mnifest items with a media-type of "application/xhtml+xml" or "text/html".
    static bool ShouldApply(const ManifestItem* item, const EncryptionInfo* encInfo);
    
public:
    /**
     Initializes a preprocessor and associates it with a Package object, from which
     it can obtain foreign media handler details.
     @param pkg The package to which this filter will apply.
     */
    ObjectPreprocessor(const Package* pkg, const string& openButtonTitle = "Open Fullscreen");
    
    ///
    /// No default constructor.
    ObjectPreprocessor()                                    = delete;
    
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
    virtual bool    RequiresCompleteData()      const   { return true; }
    
    /**
     Performs the static replacement of `object` tags whose `type` attribute
     identifies a media-type for which the Publication provides a media handler.
     
     The entire `object` element will be replaced wholesale with two elements: an
     `iframe` containing the handler and a `form` containing a `button` element
     which will open the handler full-screen.
     
     If the source `object` tag has an `id` attribute, that id will be attached to
     the new `iframe` element.  Additionally, the id will be suffixed with `-form`
     and `-button` and applied to the `form` and `button` elements respectively.  It
     is our intention that these rules will make it possible for content authors to
     anticipate these substitutions and build CSS or JavaScript rules directly.
     */
    virtual void*   FilterData(void* data, size_t len, size_t* outputLen);
    
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
    std::regex                              _objectMatcher;
    
    ///
    /// The (hopefully localized!) title of the generated HTML5 `<button>`.
    const string                            _button;
    
    ///
    /// The object keeps its own list of handlers, used to create target URIs.
    std::map<string, const MediaHandler>    _handlers;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__object_preprocessor__) */
