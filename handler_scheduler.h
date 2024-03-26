/*
 * @Author: lourisxu
 * @Date: 2024-03-26 07:09:59
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-26 07:48:05
 * @FilePath: /pipeline/handler_scheduler.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_HANDLER_SCHEDULER_H_
#define PIPELINE_HANDLER_SCHEDULER_H_

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "comm/blocking_queue.h"
#include "comm/defines.h"
#include "handler.h"

namespace PIPELINE {

// 处理器的调度适配器
class HandlerScheduler : Handler {
 public:
  // 调度
  void Schedule();

 private:
  void startTasks();

 private:
  std::vector<BlockingQueue<ChannelData>> ins_;   // 扇入通道
  std::vector<BlockingQueue<ChannelData>> outs_;  // 扇出通道
  BlockingQueue<ChannelData> merged_in_;          // 中心式合并通道
  std::vector<bool> closed_index_;                // 已关闭的入通道索引
  bool started_;                                  // 是否调度中
  std::atomic_int handle_num_;                    // 已处理数
  std::mutex mtx_;                                // 互斥量
  std::condition_variable cond_;                  // 条件变量
};

}  // namespace PIPELINE

#endif  // PIPELINE_HANDLER_SCHEDULER_H_
