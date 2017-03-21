//
//  filter_manager_impl.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 8/1/13.
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

