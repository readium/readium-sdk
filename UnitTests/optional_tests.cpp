//
//  optional_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 12/12/2013.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//


#include "../ePUb3/utilities/optional.h"
#include <string>
#include <vector>
#include "catch.hpp"

using namespace EPUB3_NAMESPACE;

TEST_CASE("default-constructed std::optional<> == nullopt", "[std::optional]")
{
    optional<std::string> opt;
    REQUIRE(opt == nullopt);
    REQUIRE_THROWS_AS(opt.value(), bad_optional_access);
}

TEST_CASE("std::optional can be assigned a value and nullopt", "[std::optional]")
{
    optional<bool> opt;
    REQUIRE(opt == nullopt);
    
    opt = true;
    REQUIRE(opt == true);
    REQUIRE(opt != false);
    REQUIRE(opt != nullopt);
    REQUIRE(opt.value() == true);
    
    opt = nullopt;
    REQUIRE(opt != true);
    REQUIRE(opt != false);
    REQUIRE(opt == nullopt);
    
    REQUIRE_THROWS_AS(opt.value(), bad_optional_access);
}

TEST_CASE("std::optional can be accessed like a pointer", "[std::optional]")
{
    std::string aStr("Hello, world!");
    optional<std::string> opt(aStr);
    
    REQUIRE(opt != nullopt);
    REQUIRE_NOTHROW(opt.value());
    
    REQUIRE(opt == aStr);
    REQUIRE(*opt == aStr);
    REQUIRE(opt->length() == aStr.length());
}

TEST_CASE("std::optional supports std::swap", "[std::optional][std::swap]")
{
    std::string str1("hello"), str2("world");
    optional<std::string> opt1(str1), opt2(str2);
    
    REQUIRE(opt1 == str1);
    REQUIRE(opt2 == str2);
    
    REQUIRE_NOTHROW(std::swap(opt1, opt2));
    
    REQUIRE(opt1 == str2);
    REQUIRE(opt2 == str1);
}

TEST_CASE("std::optional supports in-place construction of contained values", "[std::optional]")
{
    const char* str = "Hello, world!";
    optional<std::string> opt(in_place, str, 13);
    
    REQUIRE(opt != nullopt);
    REQUIRE(*opt == str);
    REQUIRE(opt->length() == 13);
}

TEST_CASE("std::optional supports emplacement value setting", "[std::optional][emplace]")
{
    const char* str = "Hello, world!";
    optional<std::string> opt;
    
    REQUIRE(opt == nullopt);
    
    REQUIRE_NOTHROW(opt.emplace(str, 13));
    REQUIRE(opt != nullopt);
    REQUIRE(*opt == str);
    REQUIRE(opt->length() == 13);
}

TEST_CASE("std::optional supports in-place construction with an initializer list", "[std::optional][std::initializer_list]")
{
    std::vector<int> compare{1, 2, 3};
    optional<std::vector<int>> opt(in_place, {1, 2, 3});
    
    REQUIRE(opt != nullopt);
    REQUIRE(*opt == compare);
    REQUIRE(opt->size() == 3);
    REQUIRE((*opt)[1] == 2);
}

TEST_CASE("std::optional supports emplacement value setting with an initializer list", "[std::optional][emplace][std::initializer_list]")
{
    std::vector<int> compare{1, 2, 3};
    optional<std::vector<int>> opt;
    
    REQUIRE(opt == nullopt);
    
    REQUIRE_NOTHROW(opt.emplace({1, 2, 3}));
    REQUIRE(opt != nullopt);
    REQUIRE(*opt == compare);
    REQUIRE(opt->size() == 3);
    REQUIRE((*opt)[1] == 2);
}

TEST_CASE("std::optional supports copy construction/assignment", "[std::optional]")
{
    const optional<bool> opt1(true);
    optional<bool> opt2(opt1);
    
    REQUIRE(opt1 == opt2);
    
    optional<bool> opt3;
    REQUIRE_NOTHROW(opt3 = opt1);
    
    REQUIRE(opt1 == opt3);
}

TEST_CASE("std::optional supports move construction/assignment", "[std::optional][std::move]")
{
    const std::string val("Hello, world!");
    optional<std::string> opt1(val);
    REQUIRE(opt1 != nullopt);
    
    optional<std::string> opt2(std::move(opt1));
    // the *value* contents are moved -- opt1 is still engaged, but contains an
    //  empty instance of its value_type.
    REQUIRE(opt1 != nullopt);
    REQUIRE(opt1->empty());
    REQUIRE(opt2 == val);
    
    optional<std::string> opt3;
    REQUIRE(opt3 == nullopt);
    
    opt3 = std::move(opt2);
    REQUIRE(opt2 != nullopt);
    REQUIRE(opt2->empty());
    REQUIRE(opt3 != nullopt);
    REQUIRE(opt3 == val);
}

TEST_CASE("std::optional can construct by moving a value", "[std::optional][std::move]")
{
    std::string val("Hello, world!");
    optional<std::string> opt1(std::move(val));
    REQUIRE(opt1 != nullopt);
    REQUIRE(val.empty());
    REQUIRE(opt1->length() == 13);
    REQUIRE(*opt1 == "Hello, world!");
}

TEST_CASE("std::optional can be disengaged by using 'opt = {}' syntax", "[std::optional]")
{
    optional<bool> opt(true);
    REQUIRE(opt != nullopt);
    
    opt = {};
    REQUIRE(opt == nullopt);
}

TEST_CASE("std::optional can be tested for engaged state by casting to bool", "[std::optional]")
{
    optional<int> opt(42);
    REQUIRE(bool(opt) == true);
    
    opt = nullopt;
    REQUIRE(bool(opt) == false);
}

TEST_CASE("std::optional::value_or always returns a value", "[std::optional]")
{
    optional<int> opt1(42);
    optional<int> opt2;
    
    REQUIRE(opt1.value_or(0x0badf00d) == 42);
    REQUIRE(opt2.value_or(0x0badf00d) == 0x0badf00d);
}
