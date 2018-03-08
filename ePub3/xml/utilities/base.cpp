//
//  base.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-11-20.
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
