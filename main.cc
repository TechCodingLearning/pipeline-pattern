/*
 * @Author: lourisxu
 * @Date: 2024-04-21 19:19:48
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 19:42:46
 * @FilePath: /pipeline/main.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include <csignal>
#include <ctime>
#include <future>
#include <iostream>
#include <string>
#include <thread>

#include "comm/debug.h"
#include "comm/functional.h"
#include "pipeline/pipeline.h"
#include "pipeline/process.h"

using namespace std;

int main() {
  PIPELINE::Pipeline* p = PIPELINE::NewPipeline();

  p->AddStage(PIPELINE::NewStage(
      "stage1", PIPELINE::NewHandler(
                    "handler1", 1, nullptr, 0, 1,
                    [](const PIPELINE::ChannelData& chan_data) {
                      std::vector<PIPELINE::ChannelData> res;
                      for (int i = 0; i < 2; i++) {
                        res.push_back(PIPELINE::ChannelData(0, new int(i)));
                      }
                      // 任务处理结束标志 ChannelData.data == nullptr
                      res.push_back(PIPELINE::ChannelData(0, nullptr));
                      return res;
                    })));
  p->AddStage(PIPELINE::NewStage(
      "stage2",
      PIPELINE::NewHandler(
          "handler2", 5, nullptr, 1, 0,
          [](const PIPELINE::ChannelData& chan_data) {
            std::vector<PIPELINE::ChannelData> res;

            if (!chan_data.IsEnd()) {
              //  std::this_thread::sleep_for(
              //      std::chrono::microseconds(100));
              cout << ">>>>>>handleFunc:" << endl;
              int* a = static_cast<int*>(chan_data.data);
              // (*a) += 99;
              const PIPELINE::PipeClosue<PIPELINE::AddOneFunc> add_one_func;
              int number = (*a) | add_one_func | add_one_func | add_one_func;

              cout << ">>>>>>>handleFunc:" << number << endl << endl;
            }
            // 结束
            return res;
          })));
  cout << p->String() << endl;

  PIPELINE::EnableDebug();
  try {
    p->Process();
  } catch (std::exception& e) {
    cerr << "Exception: " << e.what() << endl;
  }

  return 0;
}