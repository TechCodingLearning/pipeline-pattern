/*
 * @Author: lourisxu
 * @Date: 2024-03-27 08:15:21
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 19:35:14
 * @FilePath: /pipeline/pipeline/handler_scheduler.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "handler_scheduler.h"

#include <ctime>
#include <exception>
#include <future>
// #include <stdexcept>
#include <string>

#include "comm/debug.h"
#include "comm/functional.h"
#include "comm/utils.h"

namespace PIPELINE {

HandlerScheduler::HandlerScheduler(
    Handler *handler, const std::vector<BlockingQueue<ChannelData> *> &in_chans,
    const std::vector<BlockingQueue<ChannelData> *> &out_chans) {
  this->handler_ = handler;
  this->ins_ = in_chans;
  this->outs_ = out_chans;
  this->started_ = false;
  this->closed_index_ = std::vector<bool>(in_chans.size(), false);
  this->handle_num_ = std::atomic_bool(false);
  this->timeout_ = std::chrono::milliseconds(3000);  // 默认3000ms超时时间
}

HandlerScheduler::~HandlerScheduler() {}

void HandlerScheduler::Schedule(std::promise<bool> promise, int thread_idx) {
  try {
    if (this->started_) {
      throw std::runtime_error(pprintf("HandlerScheduler %s taskNum:%d",
                                       this->handler_->Name().c_str(),
                                       this->handler_->TaskNum()));
    }

    // 开始执行任务
    this->started_ = true;
    this->StartTasks();

    promise.set_value(true);
  } catch (const std::future_error &e) {
    std::cerr << "Run Scheduler FutureError: " << e.what() << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
    throw std::runtime_error(e.what());
  } catch (const std::exception &e) {
    std::cerr << "Run Scheduler ExceptionErr: " << e.what() << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
    throw std::runtime_error(e.what());
  } catch (...) {
    std::cout << this->handler_->Name() << " " << "Unknown Exception"
              << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
    throw std::runtime_error("Schedule Unknown exception");
  }
}

void HandlerScheduler::StartTasks() {
  if (this->handler_->TaskNum() <= 0) {
    throw std::runtime_error(pprintf("HandlerScheduler %s taskNum:%d",
                                     this->handler_->Name().c_str(),
                                     this->handler_->TaskNum()));
  }

  DDDDDebug("HandlerScheduler %s start tasks\n",
            this->handler_->Name().c_str());

  int n_thread = this->handler_->TaskNum() + 1;
  std::vector<std::promise<bool>> result_promises(n_thread);
  std::vector<std::future<bool>> result_futures(n_thread);
  std::vector<std::thread> threads;

  // 绑定扇入通道处理函数
  result_futures[0] = result_promises[0].get_future();
  threads.push_back(std::thread(&HandlerScheduler::SelectAndResendData, this,
                                std::move(result_promises[0]), 0));

  // 绑定扇出通道处理函数
  for (int i = 1; i < n_thread; i++) {
    result_futures[i] = result_promises[i].get_future();
    threads.push_back(std::thread(&HandlerScheduler::RunTask, this,
                                  std::move(result_promises[i]), i));
  }

  // 阻塞式捕获子线程的异常
  try {
    for (int i = 0; i < n_thread; i++) {
      result_futures[i].get();  // 子线程如果发生异常，则抛出
    }

    // 阻塞等待所有线程执行完成
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    // 关闭扇出通道，以便作为下一个Stage的Handle扇入通道识别关闭动作结束读取数据
    for (auto &bq : this->outs_) {
      bq->Close();
    }

  } catch (const std::future_error &e) {
    std::cerr << "Run StartTasks FutureError: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  } catch (const std::exception &e) {
    std::cerr << "Run StartTasks Exception caught: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  }
}

void HandlerScheduler::WaitUntil(const int &expect_n, std::string scene) {
  std::cout << this->handler_->Name() << " " << "======= waitUntil start"
            << std::endl;

  char timeString[100];
  std::chrono::system_clock::time_point curTime =
      std::chrono::system_clock::now();
  // 将时间点转换为时间结构体
  std::time_t currentTime = std::chrono::system_clock::to_time_t(curTime);
  std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S",
                std::localtime(&currentTime));
  DDDDDebug(
      "HandlerScheduler %s waitUntil scene:%s expectN:%d handlerNum:%d time: "
      "%s",
      this->handler_->Name().c_str(), scene.c_str(), expect_n,
      this->handle_num_.load(), std::string(timeString).c_str());

  std::unique_lock<std::mutex> lock(this->mtx_);
  while (expect_n != this->handle_num_.load()) {
    this->cond_.wait(lock);
  }

  return;
}

void HandlerScheduler::NotifyCondSignal() {
  std::lock_guard<std::mutex> lock(this->mtx_);
  this->cond_.notify_one();
}

void HandlerScheduler::SelectAndResendData(std::promise<bool> promise,
                                           int thread_idx) {
  try {
    int n = 0;
    while (true) {
      auto [data, done] = this->SelectData();

      DDDDDebug(
          "HandlerScheduler %s selectAndResendData selectData data:%s,done:%d",
          this->handler_->Name().c_str(), data.String().c_str(), done);

      if (done) {
        DDDDDebug("HandlerScheduler %s Select Done n:%d handle_num:%d",
                  this->handler_->Name().c_str(), n, this->handle_num_.load());

        // 等待处理完成
        // 只有当有扇入通道的时候才阻塞；如果仅有扇出通道没有扇入通道，不校验
        if (this->ins_.size() > 0) {
          this->WaitUntil(n, "done");
        }

        promise.set_value(true);

        DDDDDebug("HandlerScheduler %s selectAndResendData done",
                  this->handler_->Name().c_str());

        // 最后关闭中心合并通道
        this->merged_in_.Close();

        return;
      }

      // 阻塞式push
      this->merged_in_.Push(data);

      n++;
    }

    // 最后关闭中心合并通道
    this->merged_in_.Close();
    promise.set_value(true);
  } catch (const std::exception &e) {
    std::cerr << "run SelectAndResendData exception:" << e.what() << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
    throw std::runtime_error(e.what());
  }
}

std::tuple<ChannelData, bool> HandlerScheduler::SelectData() {
  std::vector<BlockingQueue<ChannelData> *> selected_chans;
  std::vector<int> selected_indexes(this->ins_.size());
  bool all_closed = true;

  DDDDDebug("HandlerScheduler %s selectData closedIndex: %s",
            this->handler_->Name().c_str(),
            pprintf(this->closed_index_).c_str());

  for (int i = 0; i < this->ins_.size(); i++) {
    if (this->closed_index_[i]) {
      continue;
    }

    all_closed = false;

    selected_indexes[selected_chans.size()] = i;
    selected_chans.push_back(this->ins_[i]);
  }

  if (all_closed) {
    DDDDDebug("HandlerScheduler %s selectData allClose so return done",
              this->handler_->Name().c_str());
    return {ChannelData{-1, nullptr}, true};
  }

  // 从扇入通道选择数据
  auto [chosen_index, data, ok] =
      Select(this->handler_->Name(), selected_chans, this->timeout_);

  int index = selected_indexes[chosen_index];

  if (!ok) {
    this->closed_index_[index] = true;  // 已关闭
    return this->SelectData();
  }

  // 重新编号
  data.index = index;
  return {data, false};
}

void HandlerScheduler::RunTask(std::promise<bool> promise, int task_idx) {
  DDDDDebug("HandlerScheduler RunTask %s[%d] start",
            this->handler_->Name().c_str(), task_idx);
  try {
    while (true) {
      auto [data, task_done] = this->TaskSelectData();

      // 限流
      this->TaskRateLimit(task_idx, data);

      // 处理数据
      DataSlice out_data = this->Handle(data);

      // 扇出
      bool done = this->TaskDealOutData(out_data);

      if (done) {
        task_done = true;
      }

      // 已处理数自增1
      this->handle_num_.fetch_add(1);
      this->NotifyCondSignal();  // 通知条件变量

      if (task_done || this->TaskDetectDone(task_idx)) {
        break;
      }
    }

    this->NotifyCondSignal();

    DDDDDebug("HandlerScheduler %s[%d] done", this->handler_->Name().c_str(),
              task_idx);

    promise.set_value(true);  // 执行成功
  } catch (const std::exception &e) {
    std::cerr << "run task exception:" << e.what() << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
    throw std::runtime_error(e.what());
  }
}

std::tuple<ChannelData, bool> HandlerScheduler::TaskSelectData() {
  if (this->ins_.size() == 0) {
    return {ChannelData(-1, nullptr), true};  // 扇入通道为空，任务结束
  }

  /*
    selectAndSendData和RunTask并发情况下，会出现merged_in已经关闭且为空，
    然后死锁在this->merged_in_.WaitAndPop(data);

    // // 中心式合并通道已关闭，并且数据已经处理完毕
    // if (this->merged_in_.Closed() && this->merged_in_.Empty()) {
    //   std::cout << this->handler_->Name() << " " << "merged_channel closed!"
    //             << std::endl;
    //   return {ChannelData(-1, nullptr), true};
    // }

    // std::cout << this->handler_->Name() << " " << "task select data
    eeasasssss"
    //           << std::endl;

    // // 阻塞式获取待处理数据
    // ChannelData data;
    // this->merged_in_.WaitAndPop(data);
  */

  std::vector<BlockingQueue<ChannelData> *> merge_chans;
  merge_chans.push_back(&this->merged_in_);
  auto [chosen_index, data, ok] =
      Select(this->handler_->Name(), merge_chans, this->timeout_);

  // 中心式合并通道已关闭且已为空
  if (!ok) {
    return {ChannelData(-1, nullptr), true};
  }

  return {data, false};
}

void HandlerScheduler::TaskRateLimit(int task_index,
                                     const ChannelData &chan_data) {
  if (chan_data.IsEnd()) {
    return;
  }

  if (dynamic_cast<HandlerSupportNeedLimit *>(this->handler_)) {
    HandlerSupportNeedLimit *limit_handler =
        dynamic_cast<HandlerSupportNeedLimit *>(this->handler_);
    if (!limit_handler->NeedLimit()) {
      return;
    }

    Limiter *limiter = limit_handler->GetLimiter();
    limiter->Acquire();  // 限流
  }
  return;
}

bool HandlerScheduler::TaskDealOutData(const DataSlice &data_list) {
  bool done = false;
  for (auto &data : data_list) {
    if (data.IsEnd()) {
      done = true;
      continue;
    }

    // 数据输出通道索引非法
    if (data.index >= this->handler_->OutChanNum()) {
      throw std::runtime_error(
          pprintf("HandlerScheduler %s outData.Index:%d >= OutChanNum: %d",
                  this->handler_->Name().c_str(), data.index,
                  this->handler_->OutChanNum()));
    }

    this->outs_[data.index]->Push(data);
  }

  return done;
}

bool HandlerScheduler::TaskDetectDone(int task_idx) {
  if (dynamic_cast<HandlerSupportDone *>(this->handler_)) {
    HandlerSupportDone *done_handler =
        dynamic_cast<HandlerSupportDone *>(this->handler_);
    if (done_handler->Done()) {
      DDDDDebug("HandlerScheduler %s[%d] HandlerSupportDone is Done.",
                this->handler_->Name().c_str(), task_idx);
      return true;
    }
  }
  return false;
}

DataSlice HandlerScheduler::Handle(const ChannelData &chan_data) {
  return this->handler_->Handle(chan_data);
}
}  // namespace PIPELINE
