/*
 * @Author: lourisxu
 * @Date: 2024-03-24 20:22:41
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 17:30:49
 * @FilePath: /pipeline/pipeline/handler.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_HANDLER_H_
#define PIPELINE_HANDLER_H_

#include <functional>
#include <iostream>
#include <string>

#include "comm/defines.h"
#include "comm/limiter.h"

namespace PIPELINE {

// 函数指针
// typedef DataSlice (*HandleFunc)(const ChannelData& chanData);
typedef std::function<DataSlice(const ChannelData& chanData)> HandleFunc;

// 处理器抽象基类
class Handler {
 public:
  virtual std::string Name() = 0;

  virtual int TaskNum() = 0;

  virtual int InChanNum() = 0;

  virtual int OutChanNum() = 0;

  virtual DataSlice Handle(const ChannelData& chanData) = 0;

  virtual ~Handler(){};
};

// 带限流的处理器抽象基类
class HandlerSupportNeedLimit : public Handler {
 public:
  virtual bool NeedLimit() = 0;
  virtual Limiter* GetLimiter() = 0;
};

// 支持主动结束的处理器抽象基类
class HandlerSupportDone : Handler {
 public:
  virtual bool Done() = 0;
};

// 处理器基础类
class HandlerBase : public HandlerSupportNeedLimit {
 public:
  HandlerBase(std::string name, int task_num, Limiter* limiter, int in_chan_num,
              int out_chan_num)
      : name_(name),
        task_num_(task_num),
        limiter_(limiter),
        in_chan_num_(in_chan_num),
        out_chan_num_(out_chan_num),
        done_(false) {}

  virtual ~HandlerBase(){};

  std::string Name() override { return this->name_; }

  int TaskNum() override { return this->task_num_; }

  int InChanNum() override { return this->in_chan_num_; }

  int OutChanNum() override { return this->out_chan_num_; }

  virtual DataSlice Handle(const ChannelData& chan_data) override { return {}; }

  bool NeedLimit() override { return this->limiter_ != nullptr; }

  Limiter* GetLimiter() override { return this->limiter_; }

  virtual bool Done() { return this->done_.load(); }
  virtual void SetDone(bool done) { this->done_.store(done); }

 protected:
  std::string name_;
  int task_num_;
  Limiter* limiter_;
  int in_chan_num_;
  int out_chan_num_;
  std::atomic<bool> done_;

  HandlerBase(const HandlerBase&) = delete;             // 禁用
  HandlerBase& operator=(const HandlerBase&) = delete;  // 禁用
};

}  // namespace PIPELINE

#endif  // PIPELINE_HANDLER_H_
