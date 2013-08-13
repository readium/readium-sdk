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

EPUB3_BEGIN_NAMESPACE

class FilterManagerImpl : public FilterManager
{
public:
    
    FilterManagerImpl() {}
    virtual ~FilterManagerImpl() {}
    
    virtual ContentFilter *GetFilter(const ManifestItem *item, const EncryptionInfo *encInfo, const ContentFilter::ConstructorParameters *parameters);
    virtual void RegisterFilter(ContentFilter::TypeSnifferFn sniffer, ContentFilter::TypeFactoryFn factory);
    
private:
    
    FilterManagerImpl(const FilterManagerImpl &o) _DELETED_;
    FilterManagerImpl(FilterManagerImpl &&o) _DELETED_;
    
    unique_ptr<FilterManager::Record> m_filterRecord;
    
};

EPUB3_END_NAMESPACE

#endif
