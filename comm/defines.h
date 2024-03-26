/*
 * @Author: lourisxu
 * @Date: 2024-03-24 20:34:20
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-24 23:55:19
 * @FilePath: /pipeline/comm/defines.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_COMM_DEFINES_H_
#define PIPELINE_COMM_DEFINES_H_

#include <iostream>

namespace PIPELINE {

// 通道数据类型
struct ChannelData {
  int index;
  void* data;
  ChannelData(int idx, void* data) : index(idx), data(data) {}

  void* GetData() { return data; };

  int GetIndex() { return index; };

  bool IsEnd() { return data == nullptr; }
};

typedef ChannelData* DataSlice;

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_DEFINES_H_
