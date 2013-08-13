//
//  container_constructor_parameter.h
//  ePub3
//
//  Created by Bluefire MBP2 on 7/30/13.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#ifndef ePub3_container_constructor_parameter_h
#define ePub3_container_constructor_parameter_h

#include <ePub3/filter.h>

EPUB3_BEGIN_NAMESPACE

class ContainerConstructorParameter : public ContentFilter::ConstructorParameters
{
public:
    
    ContainerConstructorParameter(Container *container) : m_container(container) {}
    ContainerConstructorParameter(const ContainerConstructorParameter &o) : m_container(o.m_container) {}
    ContainerConstructorParameter(ContainerConstructorParameter &&o)
    {
        m_container = o.m_container;
        o.m_container = nullptr;
    }
    virtual ~ContainerConstructorParameter() {}
    
    Container *GetContainer() const { return m_container; }
    
private:
    
    ContainerConstructorParameter() _DELETED_;
    
    Container *m_container;
};

EPUB3_END_NAMESPACE

#endif
