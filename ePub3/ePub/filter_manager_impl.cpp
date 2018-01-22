//
//  filter_manager_impl.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 8/1/13.
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

#include "filter_manager_impl.h"
#include "filter_chain.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

ContentFilterPtr FilterManagerImpl::GetFilterByName(const string& name, ConstPackagePtr package) const
{
    for ( auto& item : m_registeredFilters )
    {
        if ( item.GetFilterName() == name )
            return item.CreateFilter(package);
    }
    return nullptr;
}

void FilterManagerImpl::RegisterFilter(const string& name, ContentFilter::FilterPriority priority, ContentFilter::TypeFactoryFn factory)
{
    m_registeredFilters.emplace(name, priority, factory);
}

FilterChainPtr FilterManagerImpl::BuildFilterChainForPackage(ConstPackagePtr package) const
{
    shared_vector<ContentFilter> filters;
    for ( auto& record : m_registeredFilters )
    {
        ContentFilterPtr filter = record.CreateFilter(package);
        if ( filter )
            filters.push_back(filter);
    }
    
    return std::make_shared<FilterChain>(filters); //FilterChain::New(filters);
}

EPUB3_END_NAMESPACE

