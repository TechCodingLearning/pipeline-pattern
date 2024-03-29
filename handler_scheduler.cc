/*
 * @Author: lourisxu
 * @Date: 2024-03-27 08:15:21
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-29 17:16:43
 * @FilePath: /pipeline/handler_scheduler.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "handler_scheduler.h"

#include <stdexcept>
#include <string>

#include "functional.h"

namespace PIPELINE {

HandlerScheduler::HandlerScheduler(
    Handler *handler, std::vector<BlockingQueue<ChannelData> *> in_chans,
    std::vector<BlockingQueue<ChannelData> *> out_chans) {
  this->handler_ = handler;
  this->ins_ = in_chans;
  this->outs_ = out_chans;
  this->started_ = false;
  this->closed_index_ = std::vector<bool>(in_chans.size(), false);
  this->handle_num_ = std::atomic_bool(false);
}

HandlerScheduler::~HandlerScheduler() {}

void HandlerScheduler::Schedule() {
  if (this->started_) {
    throw std::runtime_error(pprintf("HandlerScheduler %s taskNum:%d",
                                     this->handler_->Name(),
                                     this->handler_->TaskNum()));
  }
  // 开始执行任务
  this->started_ = true;
  this->StartTasks();
}

void HandlerScheduler::StartTasks() {
  if (this->handler_->TaskNum() <= 0) {
    throw std::runtime_error(pprintf("HandlerScheduler %s taskNum:%d",
                                     this->handler_->Name(),
                                     this->handler_->TaskNum()));
  }

  DDDDDebug("HandlerScheduler %s start tasks", this->handler_->Name());

  int n_thread = this->handler_->TaskNum() + 1;
  std::vector<std::promise<bool>> result_promises(n_thread);
  std::vector<std::future<bool>> result_futures(n_thread);
  std::vector<std::thread> threads;

  // 绑定扇入通道处理函数
  result_futures[0] = result_promises[0].get_future();
  threads.push_back(std::thread(&HandlerScheduler::SelectAndResendData, this,
                                std::move(result_promises[0]), -1));
  // 绑定扇出通道处理函数
  for (int i = 1; i < n_thread; i++) {
    result_futures[i + 1] = result_promises[i].get_future();
    threads.push_back(std::thread(&HandlerScheduler::RunTask, this,
                                  std::move(result_promises[i]), i - 1));
  }

  // 阻塞式捕获子线程的异常
  try {
    for (int i = 0; i < n_thread; i++) {
      result_futures[i].get();  // 子线程如果发生异常，则抛出
    }
  } catch (const std::future_error &e) {
    std::cerr << "FutureError: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  } catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  }

  // 阻塞等待所有线程执行完成
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

void HandlerScheduler::SelectAndResendData(std::promise<bool> promise,
                                           int thread_idx) {
  std::vector<BlockingQueue<ChannelData> *> selected_chans(this->ins_.size());
  std::vector<int> selected_indexes(this->ins_.size());
  bool all_closed = true;

  DDDDDebug("HandlerScheduler %s selectData closedIndex: %s",
            this->handler_->Name(), "Aaa");
}

void HandlerScheduler::RunTask(std::promise<bool> promise, int task_idx) {}
}  // namespace PIPELINE
