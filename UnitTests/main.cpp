//
//  main.cpp
//  UnitTests
//
//  Created by Jim Dovey on 2013-01-02.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

