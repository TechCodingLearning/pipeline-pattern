/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:18:39
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-14 23:23:55
 * @FilePath: /pipeline/pipeline/pipeline.h
 * @Description:
 *
 * Copyright (c) 2024 by ${Author}, All Rights Reserved.
 */
#ifndef PIPELINE_PIPELINE_H_
#define PIPELINE_PIPELINE_H_

#include <string>
#include <vector>

#include "comm/defines.h"
#include "handler.h"
#include "handler_impl.h"
#include "stage.h"

namespace PIPELINE {

// 新建Stage
typedef Handler* HandlerPtr;

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

}  // namespace PIPELINE

#endif  // PIPELINE_PIPELINE_H_
