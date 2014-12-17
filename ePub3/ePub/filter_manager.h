//
//  filter_manager.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
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

#ifndef ePub3_filter_manager_h
#define ePub3_filter_manager_h

#include <ePub3/epub3.h>
#include <ePub3/filter.h>
#include <ePub3/container.h>

EPUB3_BEGIN_NAMESPACE

class FilterChain;
typedef std::shared_ptr<FilterChain> FilterChainPtr;

class FilterManager
{
public:
    
    class Record
    {
    public:
        typedef ContentFilter::FilterPriority   FilterPriority;
        
        Record(const string& name, FilterPriority priority, ContentFilter::TypeFactoryFn factory) : m_name(name), m_priority(priority), m_factory(factory) {}
        Record(const Record &o) : m_name(o.m_name), m_priority(o.m_priority), m_factory(o.m_factory) {}
        Record(Record &&o) : m_name(std::move(o.m_name)), m_priority(o.m_priority), m_factory(std::move(o.m_factory)) {}
        virtual ~Record() {}
        
        const string& GetFilterName() const
        {
            return m_name;
        }
        ContentFilterPtr CreateFilter(ConstPackagePtr package) const
        {
            return m_factory(package);
        }
        
        bool operator<(const Record& o) const
        {
            return m_priority < o.m_priority;
        }
        bool operator==(const Record& o) const
        {
            return (m_priority == o.m_priority) && (m_name == o.m_name);
        }
        
    private:
        
        string                       m_name;
        FilterPriority               m_priority;
        ContentFilter::TypeFactoryFn m_factory;
        
    };

    struct PriorityOrderHighToLow : public std::binary_function<Record, Record, bool>
    {
        bool operator()(const Record& __a, const Record& __b) {
            return __b < __a;
        }
    };
    
    virtual ~FilterManager() {}
    
    static FilterManager *Instance() { return s_instance.get(); };
    
    static void SetInstance(FilterManager *newInstance) { s_instance.reset(newInstance); }
    
    virtual ContentFilterPtr GetFilterByName(const string& name, ConstPackagePtr package) const = 0;
    virtual void RegisterFilter(const string& name, ContentFilter::FilterPriority priority, ContentFilter::TypeFactoryFn factory) = 0;
    
    virtual FilterChainPtr BuildFilterChainForPackage(ConstPackagePtr package) const = 0;
    
protected:
    
    FilterManager() {}
    FilterManager(const FilterManager &o) _DELETED_;
    FilterManager(FilterManager &&o) _DELETED_;
    
private:
    
    static unique_ptr<FilterManager> s_instance;
    
};

EPUB3_END_NAMESPACE

#endif
