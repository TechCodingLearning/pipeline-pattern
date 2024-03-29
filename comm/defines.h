/*
 * @Author: lourisxu
 * @Date: 2024-03-24 20:34:20
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-27 08:05:14
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

  void* GetData() const { return data; };

  int GetIndex() const { return index; };

  bool IsEnd() const { return data == nullptr; }
};

typedef std::vector<ChannelData> DataSlice;

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_DEFINES_H_
