/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:56:51
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-29 18:07:09
 * @FilePath: /pipeline/example/component_test.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include <gtest/gtest.h>

#include <ctime>
#include <future>
#include <iostream>
#include <string>
#include <thread>

#include "comm/blocking_queue.h"
#include "comm/defines.h"
#include "comm/functional.h"
#include "comm/rate_limiter.h"
#include "debug.h"

namespace {

using namespace std;

TEST(FunctionalTest, Basic) {
  cout << "running..." << endl;
  string str = "aaaa";
  PIPELINE::ChannelData chData(0, &str);
  string dstStr = *static_cast<string*>(chData.data);
  cout << "chData.data: " << dstStr << endl;

  const PIPELINE::PipeClosue<PIPELINE::AddOneFunc> add_one_func;
  int number = 1 | add_one_func | add_one_func | add_one_func;
  cout << "number=" << number << endl;

  cout << PIPELINE::pprintf("test pprintf:%s, %d", "aaa", 27) << endl;

  std::vector<int> vec{1, 2, 3, 4};
  cout << PIPELINE::pprintf(vec) << endl;

  std::set<string> st{"aaaa", "bbbb", "cccc"};
  cout << PIPELINE::pprintf(st) << endl;

  std::list<int> lt{3, 4, 5};
  cout << PIPELINE::pprintf(lt) << endl;

  std::map<string, int> mp;
  mp["case1"] = 1;
  mp["case2"] = 2;
  cout << PIPELINE::pprintf(mp) << endl;
}

TEST(RateLimiterTest, Basic) {
  PIPELINE::RateLimiter* limiter = new PIPELINE::RateLimiter();
  limiter->SetRate(5.0);  // 每秒五个请求

  // 获取当前时间点
  std::chrono::system_clock::time_point curTime =
      std::chrono::system_clock::now();
  char timeString[100];
  for (int i = 0; i < 10; i++) {
    long x = limiter->Acquire(1);

    curTime = std::chrono::system_clock::now();
    // 将时间点转换为时间结构体
    std::time_t currentTime = std::chrono::system_clock::to_time_t(curTime);
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S",
                  std::localtime(&currentTime));
    cout << "Request: " << i + 1 << " processed. Time: " << timeString << endl;
  }
}

TEST(DebugTest, Basic) {
  PIPELINE::DDDDDebug("test debug[%s], value[%d]\n", "tom", 28);

  PIPELINE::EnableDebug();

  PIPELINE::DDDDDebug("test debug[%s], value[%d]\n", "tony", 27);
  PIPELINE::DDDDDebug("test debug[%s], value[%d]\n", "tom", 29);

  int x = 99;

  PIPELINE::DDDDDebug("test debug[%s], value[%d]\n", "tom", x);
}

void helper(int i) {
  std::this_thread::sleep_for(std::chrono::microseconds(100));
  cout << i << "-th thread exit after sleep 100ms..." << endl;
}

TEST(ThreadTest, Basic) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) {
    threads.push_back(std::thread(&helper, i));
  }

  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

void promise_helper(std::promise<bool> promise, int i) {
  bool isPass = true;
  try {
    if (i == 3) {
      throw std::runtime_error(PIPELINE::pprintf("%d-th error", i));
    }
    cout << i << "-th aaaa" << endl;
    promise.set_value(isPass);
  } catch (const std::runtime_error& e) {
    // promise.set_exception(std::current_exception());
    promise.set_exception_at_thread_exit(std::current_exception());
    // promise.set_value(i);
    // promise.set_value_at_thread_exit(i);
  }
}

TEST(FutureTest, Basic) {
  int n_threads = 5;
  vector<std::promise<bool>> result_promises(n_threads);
  vector<std::future<bool>> result_futures(n_threads);
  std::vector<std::thread> threads;
  for (int i = 0; i < n_threads; i++) {
    result_futures[i] = result_promises[i].get_future();
    threads.push_back(std::thread(&promise_helper,
                                  std::move(result_promises[i]),
                                  i));  //  一定要move，否则报错，参数匹配错误；
  }

  // std::chrono::microseconds timeout(10);l
  // std::future_status status;

  // 捕获子线程的异常
  try {
    for (int i = 0; i < n_threads; i++) {
      // 线程未执行前，获取不到Promise的结果，所以阻塞；如果子线程发生异常，get也会抛出
      bool isPass = result_futures[i].get();
      cout << i << "-th pass?: " << isPass << endl;
    }
  } catch (const std::future_error& e) {
    cerr << "FutureError" << e.what() << endl;
  } catch (const std::exception& e) {
    cerr << "Exception caught: " << e.what() << endl;
  }

  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

TEST(BlockingQueueTest, Basic) {
  PIPELINE::BlockingQueue<int> ch;
  ch.Push(1);
  ch.Push(2);
  ch.Push(3);
  int ele;
  bool ok = ch.TryAndPop(ele);
  cout << "ok:" << ok << " ele:" << ele << endl;
  ch.Close();

  try {
    ch.Push(2);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  while (!ch.Empty()) {
    ok = ch.TryAndPop(ele);
    cout << "ok:" << ok << " ele:" << ele << endl;
  }
  cout << "ch has been closed!" << endl;

  PIPELINE::BlockingQueue<int> ch1;
  ch1.Push(1);
  ch1.Push(2);

  std::vector<PIPELINE::BlockingQueue<int>*> chans;
  chans.emplace_back(&ch1);
  chans[0]->Push(3);

  while (!ch1.Empty()) {
    ok = ch1.TryAndPop(ele);
    cout << "ok:" << ok << " ele:" << ele << endl;
  }

  cout << "ch1 has been closed!" << endl;
}

}  // namespace