/*
 * @Author: lourisxu
 * @Date: 2024-04-14 14:53:59
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 23:20:36
 * @FilePath: /pipeline/example/pipeline_test.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include "pipeline/pipeline.h"

#include <gtest/gtest.h>

#include <csignal>
#include <ctime>
#include <future>
#include <iostream>
#include <string>
#include <thread>

// #include "pipeline/handler.h"
#include "comm/debug.h"
#include "pipeline/process.h"
// #include "pipeline/stage.h"

using namespace std;

void exitHandler() { std::cout << std::flush; }

TEST(SchedulerNormal, Basic) {
  return;
  PIPELINE::HandlerImpl* handler = PIPELINE::NewHandler(
      "handler1", 1, nullptr, 1, 1, [](const PIPELINE::ChannelData& chan_data) {
        std::vector<PIPELINE::ChannelData> res;
        int* a = static_cast<int*>(chan_data.data);
        (*a)++;
        cout << "handleFunc" << endl;
        res.push_back(PIPELINE::ChannelData(0, a));
        res.push_back(PIPELINE::ChannelData(0, new int(111)));
        // 如果没有关闭入通道，则任务处理结束标志为
        // ChannelData.data == nullptr
        res.push_back(PIPELINE::ChannelData(-1, nullptr));
        return res;
      });

  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> in_chans;
  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> out_chans;

  in_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());

  out_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());
  PIPELINE::HandlerScheduler scheduler(handler, in_chans, out_chans);

  in_chans[0]->Push(PIPELINE::ChannelData(0, new int(99)));
  in_chans[0]->Close();  // 不关闭则会触发select的超时退出逻辑

  cout << "before run schedule" << endl;
  PIPELINE::EnableDebug();

  // 注册终止处理函数
  std::atexit(exitHandler);

  try {
    std::promise<bool> prms;
    scheduler.Schedule(std::move(prms), 0);

    while (!out_chans[0]->Empty()) {
      PIPELINE::ChannelData ele;
      bool ok = out_chans[0]->TryAndPop(ele);
      cout << "result ok:" << ok << " ele:" << *(static_cast<int*>(ele.data))
           << endl;
    }

  } catch (std::exception& e) {
    cout << "Exception: " << e.what() << endl;
  } catch (std::runtime_error& e) {
    cout << "RuntimeError: " << e.what() << endl;
  } catch (...) {
    cout << "Unknown Exception" << endl;
  }

  cout << "after run schedule" << endl;
}

TEST(SchedulerOnlyOut, Basic) {
  // return;
  PIPELINE::HandlerImpl* handler = PIPELINE::NewHandler(
      "handler1", 1, nullptr, 0, 1, [](const PIPELINE::ChannelData& chan_data) {
        std::vector<PIPELINE::ChannelData> res;
        int* a = static_cast<int*>(chan_data.data);
        (*a)++;
        cout << "handleFunc" << endl;
        res.push_back(PIPELINE::ChannelData(0, a));
        // 任务处理结束标志 ChannelData.data == nullptr
        res.push_back(PIPELINE::ChannelData(-1, nullptr));
        return res;
      });

  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> in_chans;
  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> out_chans;

  // in_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());

  out_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());
  PIPELINE::HandlerScheduler scheduler(handler, in_chans, out_chans);

  // in_chans[0]->Push(PIPELINE::ChannelData(0, new int(99)));
  // in_chans[0]->Close();  // 不关闭则会触发select的超时退出逻辑

  cout << "before run schedule" << endl;
  PIPELINE::EnableDebug();

  // 注册终止处理函数
  std::atexit(exitHandler);

  try {
    std::promise<bool> prms;
    scheduler.Schedule(std::move(prms), 0);

  } catch (std::exception& e) {
    cout << "Exception: " << e.what() << endl;
  } catch (std::runtime_error& e) {
    cout << "RuntimeError: " << e.what() << endl;
  } catch (...) {
    cout << "Unknown Exception" << endl;
  }

  cout << "after run schedule" << endl;
}

TEST(StageTest, Basic) {
  return;
  PIPELINE::Stage* stage = PIPELINE::NewStage(
      "stage1",
      PIPELINE::NewHandler("handler1", 1, nullptr, 1, 1,
                           [](const PIPELINE::ChannelData& chan_data) {
                             std::vector<PIPELINE::ChannelData> res;
                             int* a = static_cast<int*>(chan_data.data);
                             res.push_back(PIPELINE::ChannelData(0, a));
                             // 任务处理结束标志 ChannelData.data == nullptr
                             res.push_back(PIPELINE::ChannelData(0, nullptr));
                             return res;
                           }));
  PIPELINE::EnableDebug();

  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> in_chans;
  vector<PIPELINE::BlockingQueue<PIPELINE::ChannelData>*> out_chans;

  in_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());
  out_chans.push_back(new PIPELINE::BlockingQueue<PIPELINE::ChannelData>());

  in_chans[0]->Push(PIPELINE::ChannelData(0, new int(99)));
  in_chans[0]->Close();  // 不关闭则会触发select的超时退出逻辑

  try {
    std::promise<bool> prms;
    stage->Run(std::move(prms), 0, in_chans, out_chans);
  } catch (std::exception& e) {
    cout << "Exception: " << e.what() << endl;
  } catch (std::runtime_error& e) {
    cout << "RuntimeError: " << e.what() << endl;
  } catch (...) {
    cout << "Unknown Exception" << endl;
  }
}

TEST(PipelinePrint, Basic) {
  return;
  PIPELINE::Pipeline* p = PIPELINE::NewPipeline();

  // p->AddStage(PIPELINE::NewStage(
  //     "stage1", PIPELINE::NewHandlerBase("handler1", 1, nullptr, 0, 2)));
  // p->AddStage(PIPELINE::NewStage(
  //     "stage2", PIPELINE::NewHandlerBase("handler2", 1, nullptr, 2, 0)));

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
      PIPELINE::NewHandler("handler2", 1, nullptr, 1, 0,
                           [](const PIPELINE::ChannelData& chan_data) {
                             if (!chan_data.IsEnd()) {
                               std::this_thread::sleep_for(
                                   std::chrono::microseconds(100));
                             }
                             return std::vector<PIPELINE::ChannelData>();
                           })));
  cout << p->String() << endl;

  PIPELINE::EnableDebug();
  try {
    p->Process();
  } catch (std::exception& e) {
    cerr << "Exception: " << e.what() << endl;
  } catch (std::runtime_error& e) {
    cerr << "RuntimeError: " << e.what() << endl;
  } catch (...) {
    cerr << "Unknown Exception" << endl;
  }
}