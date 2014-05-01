//
//  base.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
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

#include "base.h"
#include <ePub3/xml/document.h>
//#include "document.h"
//#include "element.h"
//#include "attribute.h"
#include <ePub3/xml/node.h>
//#include "dtd.h"
#include <libxml/globals.h>
#include <stdio.h>

EPUB3_XML_BEGIN_NAMESPACE

static xmlRegisterNodeFunc defNodeRegister = nullptr;
static xmlRegisterNodeFunc defThrNodeRegister = nullptr;
static xmlDeregisterNodeFunc defNodeDeregister = nullptr;
static xmlDeregisterNodeFunc defThrNodeDeregister = nullptr;

static void __registerNode(xmlNodePtr aNode)
{
    Node::Wrap(aNode);
    if (defNodeRegister != nullptr)
        defNodeRegister(aNode);
}

static void __deregisterNode(xmlNodePtr aNode)
{
    Node::Unwrap(aNode);
    if (defNodeDeregister != nullptr)
        defNodeDeregister(aNode);
}

static void __registerNodeThr(xmlNodePtr aNode)
{
    Node::Wrap(aNode);
    if (defThrNodeRegister != nullptr)
        defThrNodeRegister(aNode);
}

static void __deregisterNodeThr(xmlNodePtr aNode)
{
    Node::Unwrap(aNode);
    if (defThrNodeDeregister != nullptr)
        defThrNodeDeregister(aNode);
}

//#if !EPUB_COMPILER(MSVC)
//__attribute__((destructor))
//#endif
void __resetLibXMLOverrides(void)
{
    xmlRegisterNodeDefault(defNodeRegister);
    xmlThrDefRegisterNodeDefault(defThrNodeRegister);
    xmlDeregisterNodeDefault(defNodeDeregister);
    xmlThrDefDeregisterNodeDefault(defThrNodeDeregister);
    
    xmlSubstituteEntitiesDefault(0);
    xmlLoadExtDtdDefaultValue = 0;
}

//INITIALIZER(__setupLibXML)
void __setupLibXML(void)
{
    xmlInitGlobals();
    defNodeRegister = xmlRegisterNodeDefault(&__registerNode);
    defThrNodeDeregister = xmlThrDefRegisterNodeDefault(&__registerNodeThr);
    defNodeDeregister = xmlDeregisterNodeDefault(&__deregisterNode);
    defThrNodeDeregister = xmlThrDefDeregisterNodeDefault(&__deregisterNodeThr);

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
//#if EPUB_COMPILER(MSVC)
//    atexit(__resetLibXMLOverrides);
//#endif
}

EPUB3_XML_END_NAMESPACE
