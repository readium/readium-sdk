//
//  filter_manager_impl.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 8/1/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
    
    return FilterChain::New(filters);
}

EPUB3_END_NAMESPACE

