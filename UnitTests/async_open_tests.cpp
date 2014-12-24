//
//  async_open_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 10/30/2013.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePub3/ePub/container.h"
#include "catch.hpp"

using namespace ePub3;

#define EPUB_PATH "TestData/childrens-literature-20120722.epub"

TEST_CASE("opening synchronously", "")
{
    ContainerPtr container = Container::OpenContainer(EPUB_PATH);
    REQUIRE(bool(container));
}

#ifdef SUPPORT_ASYNC
TEST_CASE("opening asynchronously", "")
{
    future<ContainerPtr> future = Container::OpenContainerAsync(EPUB_PATH);
    REQUIRE(future.wait_for(std::chrono::system_clock::duration(0)) != future_status::ready);
    
    ContainerPtr container = future.get();
    REQUIRE(bool(container));
    
    container.reset();
    
    future = Container::OpenContainerAsync(EPUB_PATH, launch::deferred);
    REQUIRE(future.wait_for(std::chrono::system_clock::duration(0)) == future_status::deferred);
    
    // should only attempt to run when get() is called
    ::sleep(1);
    
    REQUIRE(future.wait_for(std::chrono::system_clock::duration(0)) == future_status::deferred);
    
    container = future.get();
    REQUIRE(bool(container));
    
    container.reset();
    
    future = Container::OpenContainerAsync("bleh.epub", launch::async);
    REQUIRE(future.wait_for(std::chrono::system_clock::duration(0)) != future_status::ready);
    
    REQUIRE_THROWS_AS(container = future.get(), std::invalid_argument);
    REQUIRE(!bool(container));
}
#endif /* SUPPORT_ASYNC */
