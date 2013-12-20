//
//  future_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-11-15.
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

#include "../ePub3/utilities/invoke.h"
#include "../ePub3/utilities/future.h"
#include <future>
#include "catch.hpp"

using namespace EPUB3_NAMESPACE;

TEST_CASE("launch::async", "async with a policy of launch::async should run on a new thread")
{
    std::thread::id my_id = std::this_thread::get_id();
    std::thread::id run_id;
    
    std::mutex mut;
    std::condition_variable cv;
    
    std::unique_lock<std::mutex> lock(mut);
    
    auto ignored = async(launch::async, [&]() {
        std::unique_lock<std::mutex> runlock(mut);
        run_id = std::this_thread::get_id();
        cv.notify_all();
    });
    
    std::cv_status st = cv.wait_for(lock, std::chrono::seconds(2));
    lock.unlock();
    
    REQUIRE(int(st) == int(std::cv_status::no_timeout));
    
    REQUIRE(my_id != run_id);
}

TEST_CASE("launch::deferred", "async with a policy of launch::deferred should run on the same thread")
{
    std::thread::id my_id = std::this_thread::get_id();
    std::thread::id run_id;
    bool itRan = false;
    
    auto future = async(launch::deferred, [&]() {
        run_id = std::this_thread::get_id();
        itRan = true;
    });
    
    std::this_thread::yield();
    REQUIRE(itRan == false);
    
    future.wait();
    REQUIRE(itRan == true);
    REQUIRE(run_id == my_id);
}

TEST_CASE("promise/future", "promises and futures should interact correctly")
{
    promise<void> aPromise;
    future<void> aFuture = aPromise.get_future();
    
    REQUIRE(aFuture.has_value() == false);
    
    aPromise.set_value();
    REQUIRE(aFuture.has_value() == true);
}

TEST_CASE("uninitialized future", "An uninitialized future should throw an exception")
{
    future<void> aFuture;
    REQUIRE_THROWS_AS(aFuture.get(), future_uninitialized);
}

TEST_CASE("promise exceptions", "A promise with an exception set should throw from future.get()")
{
    promise<void> aPromise;
    future<void> aFuture = aPromise.get_future();
    aPromise.set_exception(std::make_exception_ptr(std::invalid_argument("test")));
    
    REQUIRE_THROWS_AS(aFuture.get(), std::invalid_argument);
}

TEST_CASE("promise exceptions 2", "A promise can be given a raw exception")
{
    promise<void> aPromise;
    future<void> aFuture = aPromise.get_future();
    
    try
    {
        throw std::invalid_argument("test");
    }
    catch (std::invalid_argument& e)
    {
        aPromise.set_exception(e);
    }
    catch (std::exception& e)
    {
        aPromise.set_exception(e);
    }
    catch (...)
    {
        aPromise.set_exception(std::current_exception());
    }
    
    REQUIRE_THROWS_AS(aFuture.get(), std::invalid_argument);
}

TEST_CASE("async() exceptions", "Exceptions thrown within async() should be re-thrown from future.get()")
{
    auto future = async(launch::async, [](){
        throw std::invalid_argument("test");
    });
    
    REQUIRE_THROWS_AS(future.get(), std::invalid_argument);
}

TEST_CASE("async() return values", "return value from async() should be retusned from future.get()")
{
    auto future = async(launch::async, [](){
        return std::string("Hello, world!");
    });
    
    REQUIRE(future.get() == "Hello, world!");
}

TEST_CASE("futures support lvalue reference types", "promise/future should be bindable to lvalue references")
{
    promise<std::string&> aPromise;
    auto future = aPromise.get_future();
    
    std::string aString("Hello, world!");
    aPromise.set_value(aString);
    
    REQUIRE(future.get() == "Hello, world!");
}

TEST_CASE("unfulfilled promises", "an unfulfilled promise will set an exceptional value at destruction time")
{
    future<void> aFuture;
    {
        promise<void> aPromise;
        aFuture = aPromise.get_future();
    }
    
    REQUIRE_THROWS_AS(aFuture.get(), broken_promise);
}

TEST_CASE("double-fetched futures", "A promise will only vend its future object once.")
{
    promise<void> aPromise;
    future<void> future1, future2;
    REQUIRE_NOTHROW(future1 = aPromise.get_future());
    REQUIRE_THROWS_AS(future2 = aPromise.get_future(), future_already_retrieved);
}

TEST_CASE("double-get() future", "A future will only vend its result once")
{
    promise<void> aPromise;
    aPromise.set_value();
    
    auto future = aPromise.get_future();
    
    REQUIRE_NOTHROW(future.get());
    REQUIRE_THROWS_AS(future.get(), future_uninitialized);
}

TEST_CASE("make_ready_future", "make_ready_future vends a future with a value")
{
    auto future = make_ready_future(std::string("Hello, world!"));
    REQUIRE(future.has_value() == true);
    REQUIRE(future.get() == "Hello, world!");
}

TEST_CASE("shared_future get()", "multiple shared_future objects can each vend the same value multiple times")
{
    promise<std::string> aPromise;
    auto future1 = aPromise.get_future().share();
    auto future2 = future1;
    
    aPromise.set_value("Hello, world!");
    
    REQUIRE_NOTHROW(future1.get());
    REQUIRE_NOTHROW(future2.get());
    REQUIRE_NOTHROW(future2.get());
    
    REQUIRE(future1.get() == "Hello, world!");
    REQUIRE(future2.get() == "Hello, world!");
}

TEST_CASE("future::then()", "[future][then]")
{
    promise<std::string> aPromise;
    
    bool firstRan = false, secondRan = false;
    
    auto nextFuture = aPromise.get_future().then([&](future<std::string> f) -> bool {
        firstRan = true;
        return f.get() == "Hello";
    });
    
    auto endFuture = nextFuture.then([&](future<bool> f) -> bool {
        secondRan = true;
        return firstRan == true;
    });
    
    std::thread([&]() {
        aPromise.set_value("Hello");
    }).detach();
    
    bool output = endFuture.get();
    
    REQUIRE(firstRan == true);
    REQUIRE(secondRan == true);
    REQUIRE(output == true);
}

TEST_CASE("future::then(async)", "[future][then:async]")
{
    promise<std::string> aPromise;
    
    bool firstRan = false, secondRan = false;
    
    auto nextFuture = aPromise.get_future().then(launch::async, [&](future<std::string> f) -> bool {
        firstRan = true;
        return f.get() == "Hello";
    });
    
    auto endFuture = nextFuture.then(launch::async, [&](future<bool> f) -> bool {
        secondRan = true;
        return firstRan == true;
    });
    
    std::thread([&]() {
        aPromise.set_value("Hello");
    }).detach();
    
    bool output = endFuture.get();
    
    REQUIRE(firstRan == true);
    REQUIRE(secondRan == true);
    REQUIRE(output == true);
}

TEST_CASE("future::then(deferred) chain", "[future][deferred]")
{
    std::chrono::steady_clock::time_point firstEnter, firstExit, secondEnter, secondExit;
    bool firstRan = false, secondRan = false;
    
    auto firstFuture = async(launch::deferred, []() {
        return std::string("Hello");
    });
    
    auto nextFuture = firstFuture.then(launch::deferred, [&](future<std::string> f) -> bool {
        firstEnter = std::chrono::steady_clock::now();
        std::string value("invalid");
        value = f.get();
        firstRan = true;
        firstExit = std::chrono::steady_clock::now();
        return value == "Hello";
    });
    REQUIRE(firstFuture.valid() == false);
    
    auto endFuture = nextFuture.then(launch::deferred, [&](future<bool> f) -> bool {
        secondEnter = std::chrono::steady_clock::now();
        bool value = false;
        value = f.get();
        secondRan = true;
        secondExit = std::chrono::steady_clock::now();
        return firstRan == true && value;
    });
    REQUIRE(nextFuture.valid() == false);
    REQUIRE(endFuture.valid() == true);
    
    bool output = endFuture.get();
    
    REQUIRE(firstRan == true);
    REQUIRE(secondRan == true);
    REQUIRE(output == true);
    REQUIRE(firstEnter > secondEnter);
    REQUIRE(secondExit > firstExit);
}

TEST_CASE("future::then(async) chain", "[future][async]")
{
    std::chrono::steady_clock::time_point firstEnter, firstExit, secondEnter, secondExit;
    bool firstRan = false, secondRan = false;
    
    auto firstFuture = async(launch::async, []() {
        return std::string("Hello");
    });
    
    auto nextFuture = firstFuture.then(launch::async, [&](future<std::string> f) -> bool {
        firstEnter = std::chrono::steady_clock::now();
        std::string value("invalid");
        value = f.get();
        firstRan = true;
        firstExit = std::chrono::steady_clock::now();
        return value == "Hello";
    });
    REQUIRE(firstFuture.valid() == false);
    
    auto endFuture = nextFuture.then(launch::async, [&](future<bool> f) -> bool {
        secondEnter = std::chrono::steady_clock::now();
        bool value = false;
        value = f.get();
        secondRan = true;
        secondExit = std::chrono::steady_clock::now();
        return firstRan == true && value;
    });
    REQUIRE(nextFuture.valid() == false);
    REQUIRE(endFuture.valid() == true);
    
    bool output = endFuture.get();
    
    REQUIRE(firstRan == true);
    REQUIRE(secondRan == true);
    REQUIRE(output == true);
    REQUIRE(firstExit < secondEnter);
}
