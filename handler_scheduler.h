/*
 * @Author: lourisxu
 * @Date: 2024-03-26 07:09:59
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-03 08:47:34
 * @FilePath: /pipeline/handler_scheduler.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_HANDLER_SCHEDULER_H_
#define PIPELINE_HANDLER_SCHEDULER_H_

#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "comm/blocking_queue.h"
#include "comm/defines.h"
#include "debug.h"
#include "handler.h"
namespace PIPELINE {

// 处理器的调度适配器
class HandlerScheduler {
 public:
  HandlerScheduler(Handler* handler,
                   std::vector<BlockingQueue<ChannelData>*> in_chans,
                   std::vector<BlockingQueue<ChannelData>*> out_chans);

  ~HandlerScheduler();

  // 调度
  void Schedule();

 private:
  // 开始并发任务处理
  void StartTasks();

  // 从扇入通道选择数据发送到中心式合并通道
  void SelectAndResendData(std::promise<bool> promise, int thread_idx);

  // 从扇入通道选择数据
  std::tuple<ChannelData, bool> SelectData();

  // 等待所有数据处理完毕
  void WaitUntil(const int& expect_n, std::string scene);

  // 唤醒
  void NotifyCondSignal();

  // 处理器并发处理中心式合并通道的数据，处理结果输出到扇出通道
  void RunTask(std::promise<bool> promise, int task_idx);

  // 并发任务从中心式合并通道取数据
  std::tuple<ChannelData, bool> TaskSelectData();

  // 任务限流
  void TaskRateLimit(int task_index, const ChannelData& chan_data);

  // 任务处理数据，并将结果输出到对应的扇出通道
  bool TaskDealOutData(const DataSlice& data_list);

  // 检查是否主动停止
  bool TaskDetectDone(int task_idx);

  // 核心处理逻辑
  DataSlice Handle(const ChannelData& chan_data);

 private:
  Handler* handler_;                               // 处理器
  std::vector<BlockingQueue<ChannelData>*> ins_;   // 扇入通道
  std::vector<BlockingQueue<ChannelData>*> outs_;  // 扇出通道
  BlockingQueue<ChannelData> merged_in_;           // 中心式合并通道
  std::vector<bool> closed_index_;  // 已关闭的入通道索引
  bool started_;                    // 是否调度中
  std::atomic_int handle_num_;      // 已处理数
  std::mutex mtx_;                  // 互斥量
  std::condition_variable cond_;    // 条件变量

  HandlerScheduler(const HandlerScheduler&) = delete;             // 禁用
  HandlerScheduler& operator=(const HandlerScheduler&) = delete;  // 禁用
};

}  // namespace PIPELINE

#endif  // PIPELINE_HANDLER_SCHEDULER_H_
