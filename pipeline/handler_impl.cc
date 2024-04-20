/*
 * @Author: lourisxu
 * @Date: 2024-03-24 23:00:48
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 17:21:26
 * @FilePath: /pipeline/pipeline/handler_impl.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "handler_impl.h"

#include <iostream>

namespace PIPELINE {

// 处理器实现
// 构造函数
HandlerImpl::HandlerImpl(std::string name, int task_num, Limiter* limiter,
                         int in_chan_num, int out_chan_num, HandleFunc handle)
    : HandlerBase(name, task_num, limiter, in_chan_num, out_chan_num),
      handle_(handle),
      only_once_(false),
      ignore_end_data_(false) {}

// 析构函数
HandlerImpl::~HandlerImpl() {}

// 处理逻辑
DataSlice HandlerImpl::Handle(const ChannelData& chan_data) {
  if (this->only_once_) {
    this->SetDone(true);
  }

  if (this->ignore_end_data_ && chan_data.IsEnd()) {
    std::cout << "HandlerImpl " << this->name_
              << "Handle ignoreEndData == true && chan_data.IsEnd()"
              << std::endl;
    return {};
  }

  return this->handle_(chan_data);
}

// 设置是否仅处理一次
void HandlerImpl::SetOnlyOnce(bool only_Once) { this->only_once_ = only_Once; }

// 设置是否忽略最后一个数据
void HandlerImpl::SetIgnoreEndData(bool ignore) {
  this->ignore_end_data_ = ignore;
}

}  // namespace PIPELINE
