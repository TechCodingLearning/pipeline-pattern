/*
 * @Author: lourisxu
 * @Date: 2024-03-29 09:54:57
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 17:28:14
 * @FilePath: /pipeline/pipeline/process.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "pipeline.h"

// #include <iostream>
// using namespace std;

// int main() {
//   cout << "enter main..." << endl;
//   return 0;
// }

#ifndef PIPELINE_PIPELINE_PROCESS_H_
#define PIPELINE_PIPELINE_PROCESS_H_

namespace PIPELINE {

template <typename... HandlerPtr>
Stage* NewStage(const std::string name, HandlerPtr... handlers) {
  return new Stage(name, {handlers...});
}

// 新建HandlerBase
HandlerBase* NewHandlerBase(std::string name, int task_num, Limiter* limiter,
                            int in_chan_num, int out_chan_num) {
  return new HandlerBase(name, task_num, limiter, in_chan_num, out_chan_num);
}

DataSlice tmpHandle(const ChannelData& chanData) { return {}; }

// 新建HandlerImpl
HandlerImpl* NewHandler(std::string name, int task_num, Limiter* limiter,
                        int in_chan_num, int out_chan_num, HandleFunc handle) {
  return new HandlerImpl(name, task_num, limiter, in_chan_num, out_chan_num,
                         handle);
}

// 创建流水线
Pipeline* NewPipeline() { return new Pipeline(); }

}  // namespace PIPELINE

#endif  // PIPELINE_PIPELINE_PROCESS_H_
