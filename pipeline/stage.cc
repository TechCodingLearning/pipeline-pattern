/*
 * @Author: lourisxu
 * @Date: 2024-04-05 08:01:01
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 19:09:00
 * @FilePath: /pipeline/pipeline/stage.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "stage.h"

#include <exception>
#include <future>
#include <memory>
#include <thread>

namespace PIPELINE {

Stage::Stage(std::string name, const std::vector<Handler *> &handlers)
    : name_(name), handlers_(handlers) {}

Stage::~Stage() {}

// 扇入通道数
int Stage::InChanNum() {
  int n = 0;
  for (auto &handler : this->handlers_) {
    n += handler->InChanNum();
  }
  return n;
}

// 删除通道数
int Stage::OutChanNum() {
  int n = 0;
  for (auto &handler : this->handlers_) {
    n += handler->OutChanNum();
  }
  return n;
}

// 执行
void Stage::Run(std::promise<bool> promise, int thread_idx,
                const std::vector<BlockingQueue<ChannelData> *> &in_chans,
                const std::vector<BlockingQueue<ChannelData> *> &out_chans) {
  try {
    std::cout << "stage run" << std::endl;

    if (in_chans.size() != this->InChanNum()) {
      throw std::runtime_error("len(ins) != stage.InChanNum()");
    }

    if (out_chans.size() != this->OutChanNum()) {
      throw std::runtime_error("len(outs) != stage.OutChanNum()");
    }

    std::cout << "stage run eeeeee" << std::endl;

    int ii = 0;
    int oi = 0;
    std::vector<std::unique_ptr<HandlerScheduler>> schedulers;
    for (auto &h : this->handlers_) {
      int nii = ii + h->InChanNum();
      int noi = oi + h->OutChanNum();
      schedulers.push_back(std::make_unique<HandlerScheduler>(
          h,
          std::vector<BlockingQueue<ChannelData> *>(
              in_chans.begin() + ii, in_chans.begin() + ii + nii),
          std::vector<BlockingQueue<ChannelData> *>(
              out_chans.begin() + oi, out_chans.begin() + oi + noi)));
      ii = nii;
      oi = noi;
    }

    int n_thread = schedulers.size();
    std::vector<std::promise<bool>> result_promises(n_thread);
    std::vector<std::future<bool>> result_futures(n_thread);
    std::vector<std::thread> threads;

    for (int i = 0; i < n_thread; i++) {
      // 并发执行
      result_futures[i] = result_promises[i].get_future();
      threads.push_back(std::thread(&PIPELINE::HandlerScheduler::Schedule,
                                    std::move(schedulers[i]),
                                    std::move(result_promises[i]), i));
    }

    // 阻塞式捕获子线程的异常
    try {
      for (int i = 0; i < n_thread; i++) {
        result_futures[i].get();  // 子线程如果发生异常，则抛出
      }
    } catch (const std::future_error &e) {
      std::cerr << "Run Stage FutureError: " << e.what() << std::endl;
      throw std::runtime_error(e.what());
    } catch (const std::exception &e) {
      std::cerr << "Run Stage Exception caught: " << e.what() << std::endl;
      throw std::runtime_error(e.what());
    }

    // 阻塞等待所有线程执行完成
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

  } catch (std::exception &e) {
    std::cerr << "Run Stage Exception caughteeeee: " << e.what() << std::endl;
    promise.set_exception_at_thread_exit(std::current_exception());
  }
}
}  // namespace PIPELINE
