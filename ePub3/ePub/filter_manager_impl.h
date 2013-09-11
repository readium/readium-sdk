//
//  filter_manager_impl.h
//  ePub3
//
//  Created by Bluefire MBP2 on 8/1/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

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
    
    std::set<Record>     m_registeredFilters;
    
};

EPUB3_END_NAMESPACE

#endif
