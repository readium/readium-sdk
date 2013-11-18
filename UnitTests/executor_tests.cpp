//
//  executor_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-11-14.
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

#include <vector>
#include <set>
#include <future>
#include <CoreFoundation/CFRunLoop.h>
#include "../ePub3/utilities/executor.h"
#include "catch.hpp"

using namespace EPUB3_NAMESPACE;

static thread_pool& shared_thread_pool()
{
    static thread_pool shared_pool(2);
    return shared_pool;
}

TEST_CASE("inline_executor", "inline_executor runs code synchronously inline")
{
    bool itRan = false;
    
    inline_executor().add([&itRan]() {
        itRan = true;
    });
    
    REQUIRE(itRan == true);
}

TEST_CASE("thread_pool", "thread_pool implements a working pool of threads")
{
    thread_pool& pool = shared_thread_pool();
    bool one_ran = false;
    bool two_ran = false;
    std::condition_variable cv;
    
    auto my_id = std::this_thread::get_id();
    decltype(my_id) one_id;
    decltype(my_id) two_id;
    
    pool.add([&]() {
        one_id = std::this_thread::get_id();
        one_ran = true;
        cv.notify_all();
    });
    pool.add([&]() {
        two_id = std::this_thread::get_id();
        two_ran = true;
        cv.notify_all();
    });
    
    std::mutex mut;
    std::unique_lock<std::mutex> lock(mut);
    auto timeout = std::chrono::seconds(5);
    bool exited = cv.wait_for(lock, timeout, [&]() {
        return one_ran && two_ran;
    });
    
    lock.unlock();
    
    REQUIRE(exited == true);
    REQUIRE(one_id != my_id);
    REQUIRE(two_id != my_id);
}

TEST_CASE("thread_pool thread choice", "thread_pool will run closures on the next available thread")
{
    thread_pool& pool = shared_thread_pool();
    size_t run_count = 0;
    std::condition_variable cv;
    
    std::set<std::thread::id>       tids;
    bool unexpected = false;
    std::thread::id first_id;
    std::thread::id second_id;
    
    pool.add([&]() {
        tids.insert(std::this_thread::get_id());
        first_id = std::this_thread::get_id();
        
        auto time = std::chrono::seconds(2);
        std::this_thread::sleep_for(time);
        
        run_count++;
        cv.notify_all();
    });
    pool.add([&]() {
        tids.insert(std::this_thread::get_id());
        second_id = std::this_thread::get_id();
        unexpected = (std::this_thread::get_id() == first_id);
        run_count++;
        cv.notify_all();
    });
    pool.add([&]() {
        tids.insert(std::this_thread::get_id());
        unexpected = (std::this_thread::get_id() != second_id);
        run_count++;
        cv.notify_all();
    });
    pool.add([&]() {
        tids.insert(std::this_thread::get_id());
        unexpected = (std::this_thread::get_id() != second_id);
        run_count++;
        cv.notify_all();
    });
    
    std::mutex mut;
    std::unique_lock<std::mutex> lock(mut);
    auto timeout = std::chrono::seconds(5);
    bool exited = cv.wait_for(lock, timeout, [&]() {
        return run_count == 4;
    });
    
    lock.unlock();
    
    REQUIRE(exited == true);
    REQUIRE(tids.size() == 2);
    REQUIRE(unexpected == false);
}

TEST_CASE("thread_pool concurrency", "a thread_pool with multiple threads should be able to run multiple closures concurrently")
{
    thread_pool& pool = shared_thread_pool();
    std::mutex mut;
    std::condition_variable cv;
    std::cv_status wait_result = std::cv_status::timeout;
    
    // don't let the threads run until all is ready
    std::unique_lock<std::mutex> wait_all(mut);
    std::condition_variable complete;
    
    pool.add([&]() {
        std::unique_lock<std::mutex> lock(mut);
        wait_result = cv.wait_for(lock, std::chrono::seconds(2));
    });
    pool.add([&]() {
        std::unique_lock<std::mutex> lock(mut);
        cv.notify_all();
        complete.notify_all();
    });
    
    // let the threads run while we wait for the condition
    complete.wait_for(wait_all, std::chrono::seconds(5));
    
    REQUIRE(int(wait_result) == int(std::cv_status::no_timeout));
}

TEST_CASE("scheduled_executor", "scheduled_executor should only enqueue closures after a given duration")
{
    thread_pool& pool = shared_thread_pool();
    bool itRan = false;
    
    auto time = std::chrono::system_clock::duration(std::chrono::seconds(1));
    pool.add_after(time, [&]() {itRan = true;});
    
    auto sleepTime = std::chrono::milliseconds(500);
    std::this_thread::sleep_for(sleepTime);
    REQUIRE(itRan == false);
    
    sleepTime = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(sleepTime);
    std::this_thread::yield();
    
    REQUIRE(itRan == true);
}

TEST_CASE("loop_executor execution", "loop_executor should only run closures when requested")
{
    loop_executor loop;
    bool run = false;
    
    loop.add([&]() {
        run = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(run == false);
    
    loop.run_queued_closures();
    REQUIRE(run == true);
}

TEST_CASE("loop_executor inline","loop_executor should run closures on the thread calling run_queued_closures() etc.")
{
    loop_executor loop;
    auto tid = std::this_thread::get_id();
    decltype(tid) ctid;
    
    loop.add([&]() {
        ctid = std::this_thread::get_id();
    });
    
    loop.run_queued_closures();
    REQUIRE(ctid == tid);
}

TEST_CASE("loop_executor once", "loop_executor should be able to run one closure at a time")
{
    loop_executor loop;
    bool run_one = false;
    bool run_two = false;
    
    loop.add([&]() {
        run_one = true;
    });
    loop.add([&]() {
        run_two = true;
    });
    
    loop.try_one_closure();
    
    REQUIRE(run_one == true);
    REQUIRE(run_two == false);
    
    loop.try_one_closure();
    REQUIRE(run_two == true);
}

TEST_CASE("loop_executor once return", "loop_executor::try_one_closure() should indicate whether a closure was available and run")
{
    loop_executor loop;
    
    loop.add([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    });
    
    REQUIRE(loop.try_one_closure() == true);
    REQUIRE(loop.try_one_closure() == false);
}

TEST_CASE("serial_executor", "closures submitted to a serial_executor should never run concurrently with one another")
{
    thread_pool& pool = shared_thread_pool();
    serial_executor serial(pool);
    
    int completed = 0;
    int concurrency = 0;
    bool was_concurrent = false;
    
    std::condition_variable waiter;
    std::mutex mut;
    std::unique_lock<std::mutex> lock(mut);
    
    serial.add([&]() {
        concurrency++;
        if (concurrency > 1)
            was_concurrent = true;
        std::this_thread::yield();
        concurrency--;
        completed++;
        waiter.notify_all();
    });
    serial.add([&]() {
        concurrency++;
        if (concurrency > 1)
            was_concurrent = true;
        std::this_thread::yield();
        concurrency--;
        completed++;
        waiter.notify_all();
    });
    
    waiter.wait_for(lock, std::chrono::seconds(2), [&]() {return completed == 2;});
    REQUIRE(completed == 2);
    REQUIRE(was_concurrent == false);
}

TEST_CASE("thread_executor", "thread_executor should spawn a new thread for each closure")
{
    thread_executor exec;
    
    std::set<std::thread::id> tids;
    size_t completed = 0;
    
    std::mutex mut;
    std::condition_variable cv;
    
    executor::closure_type closure = [&](){
        tids.insert(std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        completed++;
        cv.notify_all();
    };
    
    for (int i = 0; i < 4; i++)
        exec.add(closure);
    
    std::unique_lock<std::mutex> lock(mut);
    cv.wait_for(lock, std::chrono::seconds(5), [&](){return completed == 4;});
    
    REQUIRE(completed == 4);
    REQUIRE(tids.size() == 4);
}

TEST_CASE("thread_executor reaping", "thread_executor should reap all in-flight threads in its destructor")
{
    thread_executor* pExec = new thread_executor;
    bool itRan = false;
    
    pExec->add([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        itRan = true;
    });
    
    REQUIRE(itRan == false);
    
    delete pExec;
    REQUIRE(itRan == true);
}

TEST_CASE("main_thread_executor", "main_thread_executor should run code exclusively on the application's main thread")
{
    std::shared_ptr<executor> main_exec = main_thread_executor();
    bool run_one = false;
    bool run_two = false;
    
    __block bool timeout = false;
    
    // may or may not be the main thread...
    CFRunLoopRef wait_ref = CFRunLoopGetCurrent();
    
    CFRunLoopTimerRef timer = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 5.0, 0.0, 0, 0, ^(CFRunLoopTimerRef timer) {
        timeout = true;
    });
    
    CFRunLoopRef one_ref = nullptr;
    CFRunLoopRef two_ref = nullptr;
    
    // add one directly
    main_exec->add([&]() {
        one_ref = CFRunLoopGetCurrent();
        run_one = true;
        CFRunLoopPerformBlock(wait_ref, kCFRunLoopCommonModes, ^{
            int i = 0;
            i++;
        });
        CFRunLoopWakeUp(wait_ref);
    });
    
    // add one from a guaranteed background thread
    std::async(std::launch::async, [main_exec, &two_ref, &run_two, &wait_ref]() {
        main_exec->add([&]() {
            two_ref = CFRunLoopGetCurrent();
            run_two = true;
            CFRunLoopPerformBlock(wait_ref, kCFRunLoopCommonModes, ^{
                int i = 0;
                i++;
            });
            CFRunLoopWakeUp(wait_ref);
        });
    });
    
    // spin the run loop until everything has run, or until we time out
    while (!run_one && !run_two && !timeout)
    {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.25, TRUE);
    }
    
    CFRunLoopTimerInvalidate(timer);
    CFRelease(timer);
    
    REQUIRE(run_one == true);
    REQUIRE(run_two == true);
    REQUIRE(timeout == false);
    REQUIRE(one_ref == CFRunLoopGetMain());
    REQUIRE(two_ref == CFRunLoopGetMain());
}
