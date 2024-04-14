/*
 * @Author: lourisxu
 * @Date: 2024-04-14 14:53:59
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-14 17:39:51
 * @FilePath: /pipeline/example/pipeline_test.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include "pipeline/pipeline.h"

#include <gtest/gtest.h>

#include <ctime>
#include <future>
#include <iostream>
#include <string>
#include <thread>

// #include "pipeline/handler.h"
#include "pipeline/process.h"
// #include "pipeline/stage.h"

using namespace std;

TEST(PipelinePrint, Basic) {
  PIPELINE::Pipeline* p = PIPELINE::NewPipeline();

  // p->AddStage(PIPELINE::NewStage(
  //     "stage1", PIPELINE::NewHandlerBase("handler1", 1, nullptr, 0, 2)));
  // p->AddStage(PIPELINE::NewStage(
  //     "stage2", PIPELINE::NewHandlerBase("handler2", 1, nullptr, 2, 0)));

  p->AddStage(PIPELINE::NewStage(
      "stage1", PIPELINE::NewHandler(
                    "handler1", 1, nullptr, 0, 2,
                    [](const PIPELINE::ChannelData& chan_data) {
                      std::vector<PIPELINE::ChannelData> res(2);
                      for (int i = 0; i < 2; i++) {
                        res.push_back(PIPELINE::ChannelData(0, new int(i)));
                      }
                      res.push_back(PIPELINE::ChannelData(0, nullptr));
                      return res;
                    })));
  p->AddStage(PIPELINE::NewStage(
      "stage2",
      PIPELINE::NewHandler("handler2", 5, nullptr, 2, 0,
                           [](const PIPELINE::ChannelData& chan_data) {
                             if (!chan_data.IsEnd()) {
                               std::this_thread::sleep_for(
                                   std::chrono::microseconds(100));
                             }
                             return std::vector<PIPELINE::ChannelData>();
                           })));
  cout << p->String() << endl;
}