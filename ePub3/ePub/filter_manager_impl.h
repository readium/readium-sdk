//
//  filter_manager_impl.h
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

#ifndef ePub3_filter_manager_impl_h
#define ePub3_filter_manager_impl_h

#include <ePub3/filter_manager.h>
#include <ePub3/filter.h>
#include <set>

EPUB3_BEGIN_NAMESPACE

class FilterManagerImpl : public FilterManager
{
public:
    
    FilterManagerImpl() : FilterManager(), m_registeredFilters() {}
    virtual ~FilterManagerImpl() {}
    
    virtual ContentFilterPtr GetFilterByName(const string& name, ConstPackagePtr package) const;
    virtual void RegisterFilter(const string& name, ContentFilter::FilterPriority priority, ContentFilter::TypeFactoryFn factory);
    
    virtual FilterChainPtr BuildFilterChainForPackage(ConstPackagePtr package) const;
    
private:
    
    FilterManagerImpl(const FilterManagerImpl &o) _DELETED_;
    FilterManagerImpl(FilterManagerImpl &&o) _DELETED_;
    
    std::set<Record, PriorityOrderHighToLow>     m_registeredFilters;
    
};

EPUB3_END_NAMESPACE

#endif
