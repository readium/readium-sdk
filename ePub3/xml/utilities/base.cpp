//
//  base.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "base.h"
#include "node.h"
//#include "document.h"
//#include "element.h"
//#include "attribute.h"
#include "ns.h"
//#include "dtd.h"
#include <libxml/globals.h>

EPUB3_XML_BEGIN_NAMESPACE

static xmlRegisterNodeFunc defNodeRegister = nullptr;
static xmlRegisterNodeFunc defThrNodeRegister = nullptr;
static xmlDeregisterNodeFunc defNodeDeregister = nullptr;
static xmlDeregisterNodeFunc defThrNodeDeregister = nullptr;

static void __registerNode(xmlNodePtr aNode)
{
    Node::Wrap(aNode);
}

static void __deregisterNode(xmlNodePtr aNode)
{
    Node::Unwrap(aNode);
}

__attribute__((constructor))
static void __setupLibXML(void)
{
    xmlInitGlobals();
    defNodeRegister = xmlRegisterNodeDefault(&__registerNode);
    defThrNodeDeregister = xmlThrDefRegisterNodeDefault(&__registerNode);
    defNodeDeregister = xmlDeregisterNodeDefault(&__deregisterNode);
    defThrNodeDeregister = xmlThrDefDeregisterNodeDefault(&__deregisterNode);
    
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
}

__attribute__((destructor))
static void __resetLibXMLOverrides(void)
{
    xmlRegisterNodeDefault(defNodeRegister);
    xmlThrDefRegisterNodeDefault(defThrNodeRegister);
    xmlDeregisterNodeDefault(defNodeDeregister);
    xmlThrDefDeregisterNodeDefault(defThrNodeDeregister);
    
    xmlSubstituteEntitiesDefault(0);
    xmlLoadExtDtdDefaultValue = 0;
}

EPUB3_XML_END_NAMESPACE
