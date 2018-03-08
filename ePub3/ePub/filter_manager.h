//
//  filter_manager.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
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
