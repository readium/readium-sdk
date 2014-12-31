//
//  switch_preprocessor.h
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

#ifndef __ePub3__switch_preprocessor__
#define __ePub3__switch_preprocessor__

#include <ePub3/epub3.h>
#include <ePub3/filter.h>
#include <vector>
#include REGEX_INCLUDE

EPUB3_BEGIN_NAMESPACE

/**
 A filter for preprocessing `epub:switch` compounds.
 
 The SwitchPreprocessor will locate any `<epub:switch>` compounds in an EPUB Content
 Document and will statically alter their contents by replacing each switch compound
 with the content of an epub:case element or epub:default element depending on the
 namespaces supported.
 
 Documents are identified by checking their manifest items for the `switch` property.
 If a document contains an epub:switch statement but doesn't have this property,
 then that file will be passed through unchanged.
 
 It should be used only for reading, never for writing.
 @ingroup filters
 */
class SwitchPreprocessor : public ContentFilter, public PointerType<SwitchPreprocessor>
{
public:
    ///
    /// A list of supported namespaces, as strings.
    typedef std::vector<string>     NamespaceList;
    
protected:
    /// Only documents whose manifest items are XHTML with the `switch` property
    /// will be filtered.
    static bool SniffSwitchableContent(ConstManifestItemPtr item);
    
    static ContentFilterPtr SwitchFilterFactory(ConstPackagePtr package);
    
public:
    
    /**
     The default constructor indicates that no additional content is supported, and
     the resulting filter will only preserve the content of epub:default tags.
     */
    SwitchPreprocessor() : ContentFilter(SniffSwitchableContent) {}
    
    ///
    /// The standard copy constructor.
    SwitchPreprocessor(const SwitchPreprocessor& o) : ContentFilter(o) {}
    
    ///
    /// The standard C++11 'move' constructor.
    SwitchPreprocessor(SwitchPreprocessor&& o) : ContentFilter(std::move(o)) {}
    
    /**
     This processor won't work on streamed data, it requires the whole thing at once.
     */
    virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::RequiresCompleteData; }
    
    /**
     Filters the input data using regular expressions to identify epub:switch
     compounds and replace them wholesale wih the contents of an epub:case or
     epub:default element.
     
     If the list of supported namespaces is empty, then this takes an optimized path,
     ignoring epub:case elements completely. Otherwise, it will inspect the 
     `required-namespace` attribute of each case element and will match it against
     the contents of its supported namespace list to make a decision. The first
     matching epub:case statement will be output in place of the entire switch
     compound.
     */
    virtual void * FilterData(FilterContext* context, void *data, size_t len, size_t *outputLen) OVERRIDE;
    
    ///
    /// Register this filter with the filter manager
    static void Register();
    
    ///
    /// Set the list of namespaces for content supported by the renderer.
    static void SetSupportedNamespaces(const NamespaceList& namespaces) { _supportedNamespaces = namespaces; }
    static void SetSupportedNamespaces(NamespaceList&& namespaces) { _supportedNamespaces = std::move(namespaces); }
    
protected:
    /**
     All the namespaces for content to be allowed through the filter.
     
     The default value includes **no namespaces**.
     */
    static NamespaceList    _supportedNamespaces;
    
private:
    /**
     A regex to un-comment any epub:switch blocks.
     
     For instance, we might see:
     
         <!--<epub:switch id="bob">
           <epub:case required-namespace="...">
              ...
           </epub:case>
           <epub:default>-->
             <img src="..." /><!--
           </epub:default>
         </epub:switch>-->
     
     This regex will identify such constructs, but will NOT match an entire
     commented-out epub:switch block (i.e. where the publisher has chosen to comment
     out the whole thing and provide only the default content).
     */
    static REGEX_NS::regex   CommentedSwitchIdentifier;
    
    /**
     This expression locates and isolates the content of each epub:switch element.
     */
    static REGEX_NS::regex   SwitchContentExtractor;
    
    /**
     This expression locates and extracts the contents and types of each epub:case
     element.
     */
    static REGEX_NS::regex   CaseContentExtractor;
    
    /**
     This expression locates and extracts the content of the epub:default element.
     */
    static REGEX_NS::regex   DefaultContentExtractor;
    
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__switch_preprocessor__) */
