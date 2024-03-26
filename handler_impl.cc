/*
 * @Author: lourisxu
 * @Date: 2024-03-24 23:00:48
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-26 09:36:46
 * @FilePath: /pipeline/handler_impl.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include <iostream>

#include "comm/rate_limiter.h"
#include "handler.h"

namespace PIPELINE {

// 处理器实现

// 构造函数
HandlerImpl::HandlerImpl(std::string name, int task_num, Limiter* limiter,
                         int in_chan_num, int out_chan_num,
                         PIPELINE::HandleFunc handleFunc)
    : HandlerBase(name, task_num, limiter, in_chan_num, out_chan_num),
      handle_(handleFunc),
      only_once_(false),
      ignore_end_data_(false) {}

// 析构函数
HandlerImpl::~HandlerImpl(){};

// 处理逻辑
DataSlice HandlerImpl::Handle(ChannelData* chanData) {
  if (this->only_once_) {
    this->SetDone(true);
  }

  if (this->ignore_end_data_ && chanData->IsEnd()) {
    std::cout << "HandlerImpl " << this->name_
              << "Handle ignoreEndData == true && chanData.IsEnd()"
              << std::endl;
    return nullptr;
  }

  return this->handle_(chanData);
}

// 设置是否仅处理一次
void HandlerImpl::SetOnlyOnce(bool only_Once) { this->only_once_ = only_Once; }

// 设置是否忽略最后一个数据
void HandlerImpl::SetIgnoreEndData(bool ignore) {
  this->ignore_end_data_ = ignore;
}

}  // namespace PIPELINE
