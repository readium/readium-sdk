//
//  main.cpp
//  UnitTests
//
//  Created by Jim Dovey on 2013-01-02.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#define CATCH_CONFIG_RUNNER
#include <string>
#include "catch.hpp"
#include "../ePub3/ePub/initialization.h"
#include "../ePub3/ePub/archive.h"
#include "../ePub3/xml/utilities/io.h"
#include "../ePub3/ePub/filter_manager_impl.h"

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
    
    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();
    
    /////////////////////////////////////
    // Unit Tests
    /////////////////////////////////////
    
    Catch::Session session;
    
    //session.configData().showSuccessfulTests = true;
    session.configData().showDurations = Catch::ShowDurations::Always;
    
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)
        return returnCode;
    
    int result = session.run();
    
    //////////////////////////////////////
    // global teardown here
    //////////////////////////////////////
    
    return result;
}

