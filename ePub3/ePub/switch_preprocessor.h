//
//  switch_preprocessor.h
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
