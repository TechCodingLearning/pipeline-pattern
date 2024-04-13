/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:18:39
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-14 01:07:52
 * @FilePath: /pipeline/pipeline.h
 * @Description:
 *
 * Copyright (c) 2024 by ${Author}, All Rights Reserved.
 */
#ifndef PIPELINE_PIPELINE_H_
#define PIPELINE_PIPELINE_H_

#include <string>
#include <vector>

#include "handler.h"
#include "handler_impl.h"
#include "stage.h"

namespace PIPELINE {

class Pipeline {
 public:
  Pipeline* AddStage(Stage* stage);

  template <typename... HandlerPtr>
  Pipeline* AddNewStage(std::string name, HandlerPtr... handlers);
  void Validate();
  void Process();
  std::string String();

 private:
  std::vector<Stage*> stages_;
};

// 新建Stage
typedef Handler* HandlerPtr;

template <typename... HandlerPtr>
Stage* NewStage(const std::string name,
                HandlerPtr... handlers) {  // return new Stage(name, handlers);
  return new Stage(name, {handlers...});
}

// 新建HandlerBase
HandlerBase* NewHandlerBase(std::string name, int task_num, Limiter* limiter,
                            int in_chan_num, int out_chan_num) {
  return new HandlerBase(name, task_num, limiter, in_chan_num, out_chan_num);
}

// 新建HandlerImpl
HandlerImpl* NewHandler(std::string name, int task_num, Limiter* limiter,
                        int in_chan_num, int out_chan_num, HandleFunc handle) {
  return new HandlerImpl(name, task_num, limiter, in_chan_num, out_chan_num,
                         handle);
}

// 创建流水线
Pipeline* NewPipeline() { return new Pipeline(); }

}  // namespace PIPELINE

#endif  // PIPELINE_PIPELINE_H_
