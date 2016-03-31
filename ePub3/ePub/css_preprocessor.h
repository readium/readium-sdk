//
//  css_preprocessor.hpp
//  ePub3
//
//  Created by Olivier Körner on 03/03/2016.
//  Copyright (c) 2016 The Readium Foundation and contributors. All rights reserved.
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

#ifndef __ePub3__css_preprocessor_hpp
#define __ePub3__css_preprocessor_hpp

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
class CSSPreprocessor : public ContentFilter, public PointerType<CSSPreprocessor>
{
protected:
    ///
    /// Matches only mnifest items with a media-type of "application/xhtml+xml" or "text/html" or "text/css".
    static bool ShouldApply(ConstManifestItemPtr item);
    
    /// The factory routine
    static ContentFilterPtr CSSFilterFactory(ConstPackagePtr package);
    
private:
    
    ///
    /// No default constructor.
    //CSSPreprocessor() : ContentFilter(ShouldApply) { }
    CSSPreprocessor()                                    _DELETED_;
    
    // Filter context for CSS preprocessing: is the stream CSS or HTML?
    class CSSFilterContext : public FilterContext
    {
    private:
        bool          _isCSS;
        
    public:
        CSSFilterContext() : FilterContext(), _isCSS(false) {}
        CSSFilterContext(ConstManifestItemPtr item) { _isCSS = (item->MediaType().compare("text/css") == 0); }
        
        bool  isCSS() const      { return _isCSS; }
        void setCSS(bool val)  { _isCSS = val; }
        
    };
    
public:
    class CSSSubstitution
    {
    public:
        CSSSubstitution(const REGEX_NS::regex& search_re, const std::string& replace) : m_search_re(search_re), m_replace(replace) {}
        CSSSubstitution(const CSSSubstitution &o) : m_search_re(o.m_search_re), m_replace(o.m_replace) {}
        CSSSubstitution(CSSSubstitution &&o) : m_search_re(std::move(o.m_search_re)), m_replace(std::move(o.m_replace)) {}
        virtual ~CSSSubstitution() {}
        
        const REGEX_NS::regex& GetSearchRegex() const
        {
            return m_search_re;
        }
        const std::string& GetReplaceFormat() const
        {
            return m_replace;
        }
        
    private:
        
        REGEX_NS::regex              m_search_re;
        std::string                  m_replace;
        
    };

    typedef std::vector<CSSSubstitution>     CSSSubstitutionList;

    /**
     Initializes a preprocessor and associates it with a Package object, from which
     it can obtain foreign media handler details.
     @param pkg The package to which this filter will apply.
     */
    EPUB3_EXPORT
    CSSPreprocessor(ConstPackagePtr pkg, CSSSubstitutionList substitutions);
    //CSSPreprocessor(ConstPackagePtr pkg) : ContentFilter(ShouldApply);
    
    ///
    /// Standard copy constructor.
    CSSPreprocessor(const CSSPreprocessor& o) : ContentFilter(o), m_substitutions(o.m_substitutions) {}
    
    ///
    /// C++11 'move' constructor.
    CSSPreprocessor(CSSPreprocessor&& o) : ContentFilter(std::move(o)), m_substitutions(std::move(o.m_substitutions)) {}
    
    ///
    /// Destructor.
    
    ///
    /// This preprocessor requires access to the entire content document at once.
    virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::RequiresCompleteData; }
    
    /**
     Performs the static replacement of `object` tags whose `type` attribute
     identifies a media-type for which the Publication provides a media handler.
     
     */
    virtual void*   FilterData(FilterContext* context, void* data, size_t len, size_t* outputLen) OVERRIDE;
    
    // register with the filter manager
    static void Register();

private:
    CSSSubstitutionList m_substitutions;

protected:

    virtual FilterContext *InnerMakeFilterContext(ConstManifestItemPtr item) const OVERRIDE { return new CSSFilterContext(item); }
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__css_preprocessor__) */
