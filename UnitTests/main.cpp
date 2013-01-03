//
//  main.cpp
//  UnitTests
//
//  Created by Jim Dovey on 2013-01-02.
//  Copyright (c) 2013 Kobo Inc. All rights reserved.
//

#define CATCH_CONFIG_RUNNER
#include <string>
#include "catch.hpp"
#include "../ePub3/ePub/archive.h"
#include "../ePub3/xml/utilities/io.h"

extern "C" void DumpXMLString(xmlNodePtr node)
{
    xmlOutputBufferPtr buf = xmlOutputBufferCreateFile(stdout, nullptr);
    xmlNodeDumpOutput(buf, node->doc, node, 0, 0, nullptr);
    xmlOutputBufferClose(buf);
    fprintf(stdout, "\n\n");
}

struct __tmp_
{
    bool __memb_;
};

int main(int argc, char * const argv[])
{
    //////////////////////////////////////
    // global setup here
    //////////////////////////////////////
    
    ePub3::Archive::Initialize();
    
    int result = Catch::Main(argc, argv);
    
    //////////////////////////////////////
    // global teardown here
    //////////////////////////////////////
    
    return result;
}

