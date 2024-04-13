/*
 * @Author: lourisxu
 * @Date: 2024-04-14 01:06:07
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-14 01:11:23
 * @FilePath: /pipeline/handler_impl.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_HANDLER_IMPL_H_
#define PIPELINE_HANDLER_IMPL_H_

#include "handler.h"

namespace PIPELINE {

// 处理器实现类
class HandlerImpl : public HandlerBase {
 public:
  HandlerImpl(std::string name, int task_num, Limiter* limiter, int in_chan_num,
              int out_chan_num, HandleFunc handle);

  virtual ~HandlerImpl();

  DataSlice Handle(const ChannelData& chanData);

  void SetOnlyOnce(bool only_Once);

  void SetIgnoreEndData(bool ignore);

 private:
  HandleFunc handle_;
  bool only_once_;
  bool ignore_end_data_;

  HandlerImpl(const HandlerBase&) = delete;             // 禁用
  HandlerImpl& operator=(const HandlerBase&) = delete;  // 禁用
};

}  // namespace PIPELINE

#endif  // PIPELINE_HANDLER_IMPL_H_
