//
//  filter_manager_impl.cpp
//  ePub3
//
//  Created by Bluefire MBP2 on 8/1/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "filter_manager_impl.h"

EPUB3_BEGIN_NAMESPACE

ContentFilter *FilterManagerImpl::GetFilter(const ManifestItem *item, const EncryptionInfo *encInfo, const ContentFilter::ConstructorParameters *parameters)
{
    if (m_filterRecord && m_filterRecord->IsFilterApplicable(item, encInfo))
    {
        return m_filterRecord->CreateFilter(parameters);
    }
    
    return nullptr;
}

void FilterManagerImpl::RegisterFilter(ContentFilter::TypeSnifferFn sniffer, ContentFilter::TypeFactoryFn factory)
{
    m_filterRecord.reset(new FilterManager::Record(sniffer, factory));
}

EPUB3_END_NAMESPACE

