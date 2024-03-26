/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:56:51
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-26 09:39:49
 * @FilePath: /pipeline/main.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include <any>
#include <ctime>
#include <iostream>
#include <string>

#include "comm/functional.h"
#include "comm/rate_limiter.h"
#include "debug.h"

using namespace std;

struct ChannelData {
  int index;
  void* data;
  ChannelData(int idx, void* data) : index(idx), data(data) {}
};

void testFunctional();

void testRateLimiter();

void testDebug();

int main() {
  testFunctional();

  testRateLimiter();

  testDebug();
  return 0;
}

void testFunctional() {
  cout << "running..." << endl;
  string str = "aaaa";
  ChannelData chData(0, &str);
  string dstStr = *static_cast<string*>(chData.data);
  cout << "chData.data: " << dstStr << endl;

  const PIPELINE::PipeClosue<PIPELINE::AddOneFunc> add_one_func;
  int number = 1 | add_one_func | add_one_func | add_one_func;
  cout << "number=" << number << endl;
}

void testRateLimiter() {
  PIPELINE::RateLimiter* limiter = new PIPELINE::RateLimiter();
  limiter->SetRate(5.0);  // 每秒五个请求

  // 获取当前时间点
  std::chrono::system_clock::time_point curTime =
      std::chrono::system_clock::now();
  char timeString[100];
  for (int i = 0; i < 20; i++) {
    long x = limiter->Acquire(1);

    curTime = std::chrono::system_clock::now();
    // 将时间点转换为时间结构体
    std::time_t currentTime = std::chrono::system_clock::to_time_t(curTime);
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S",
                  std::localtime(&currentTime));
    cout << "Request: " << i + 1 << " processed. Time: " << timeString << endl;
  }
}

void testDebug() {
  PIPELINE::DDDDDebug("test debug[%s], value[%d]", "tom", 28);

  PIPELINE::EnableDebug();

  PIPELINE::DDDDDebug("test debug[%s], value[%d]", "tony", 27);
}