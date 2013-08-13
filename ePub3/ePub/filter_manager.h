//
//  filter_manager.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/31/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_filter_manager_h
#define ePub3_filter_manager_h

#include <ePub3/epub3.h>
#include <ePub3/filter.h>
#include <ePUb3/container.h>

EPUB3_BEGIN_NAMESPACE

class FilterManager
{
public:
    
    class Record
    {
    public:
        
        Record(ContentFilter::TypeSnifferFn sniffer, ContentFilter::TypeFactoryFn factory) : m_sniffer(sniffer), m_factory(factory) {}
        Record(const Record &o) : m_sniffer(o.m_sniffer), m_factory(o.m_factory) {}
        Record(Record &&o) : m_sniffer(std::move(o.m_sniffer)), m_factory(std::move(o.m_factory)) {}
        virtual ~Record() {}
        
        bool IsFilterApplicable(const ManifestItem *item, const EncryptionInfo *encInfo) const
        {
            return m_sniffer(item, encInfo);
        }
        
        ContentFilter *CreateFilter(const ContentFilter::ConstructorParameters *parameters) const
        {
            return m_factory(parameters);
        }
        
    private:
        
        ContentFilter::TypeSnifferFn m_sniffer;
        ContentFilter::TypeFactoryFn m_factory;
        
    };
    
    virtual ~FilterManager() {}
    
    static FilterManager *Instance() { return s_instance.get(); };
    
    static void SetInstance(FilterManager *newInstance) { s_instance.reset(newInstance); }
    
    virtual ContentFilter *GetFilter(const ManifestItem *item, const EncryptionInfo *encInfo, const ContentFilter::ConstructorParameters *parameters) = 0;
    virtual void RegisterFilter(ContentFilter::TypeSnifferFn sniffer, ContentFilter::TypeFactoryFn factory) = 0;
    
protected:
    
    FilterManager() {}
    FilterManager(const FilterManager &o) _DELETED_;
    FilterManager(FilterManager &&o) _DELETED_;
    
private:
    
    static unique_ptr<FilterManager> s_instance;
    
};

EPUB3_END_NAMESPACE

#endif
