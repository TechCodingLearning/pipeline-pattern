/*
 * @Author: lourisxu
 * @Date: 2024-03-24 20:34:20
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 16:10:32
 * @FilePath: /pipeline/comm/defines.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_COMM_DEFINES_H_
#define PIPELINE_COMM_DEFINES_H_
#include <sstream>

namespace PIPELINE {

// 通道数据类型
struct ChannelData {
  int index;
  void* data;
  ChannelData() : index(-1), data(nullptr) {}
  ChannelData(int idx, void* data) : index(idx), data(data) {}

  void* GetData() const { return data; };

  int GetIndex() const { return index; };

  bool IsEnd() const { return data == nullptr; }

  std::string String() const {
    std::ostringstream oss;
    oss << "[index: " << index << ", data: " << data << "\n";
    return oss.str();
  }
};

typedef std::vector<ChannelData> DataSlice;

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_DEFINES_H_
