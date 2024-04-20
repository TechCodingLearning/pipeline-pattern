/*
 * @Author: lourisxu
 * @Date: 2024-04-05 08:01:30
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-15 17:26:03
 * @FilePath: /pipeline/pipeline/stage.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_STAGE_H_
#define PIPELINE_STAGE_H_

#include <string>
#include <vector>

#include "comm/blocking_queue.h"
#include "handler.h"
#include "handler_scheduler.h"

namespace PIPELINE {

class Stage {
 public:
  Stage(std::string name, const std::vector<Handler *> &handlers);

  virtual ~Stage();

  // 扇入通道数
  int InChanNum();

  // 删除通道数
  int OutChanNum();

  // 执行
  void Run(std::promise<bool> promise, int thread_idx,
           const std::vector<BlockingQueue<ChannelData> *> &in_chans,
           const std::vector<BlockingQueue<ChannelData> *> &out_chans);

 private:
  std::string name_;
  std::vector<Handler *> handlers_;
  // 友元类
  friend class Pipeline;
};

}  // namespace PIPELINE

#endif  // PIPELINE_STAGE_H_
