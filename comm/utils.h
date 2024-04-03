/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:48:58
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-31 09:31:06
 * @FilePath: /pipeline/comm/utils.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_UTILS_H_
#define PIPELINE_COMM_UTILS_H_

#include <algorithm>
#include <random>
#include <thread>
#include <tuple>
#include <vector>

#include "blocking_queue.h"
#include "defines.h"
namespace PIPELINE {
// 返回参数：被选择的通道索引，选择的数据，是否获取成功，通道是否关闭
std::tuple<int, ChannelData, bool, bool> SelectByRandom(
    std::vector<BlockingQueue<ChannelData> *> &ins) {
  std::random_device rd;  // 真随机数生成器作为种子来源
  std::mt19937 g(rd());   // 以 rd 初始化 Mersenne Twister 引擎

  std::uniform_int_distribution<int> dis(0, ins.size() - 1);

  int chosen_index = dis(g);  // 生成随机数
  ChannelData data(-1, nullptr);
  BlockingQueue<ChannelData> *ch = ins[chosen_index];
  if (ch->Closed() && ch->Empty()) {
    return {chosen_index, data, false, true};
  }

  bool ok = ch->TryAndPop(data);
  return {chosen_index, data, ok, false};
}

std::tuple<int, ChannelData, bool> Select(
    std::vector<BlockingQueue<ChannelData> *> &ins) {
  while (true) {
    auto [chosen_index, ch_data, is_get_data, is_closed] = SelectByRandom(ins);

    if (is_closed) {
      return {chosen_index, ch_data, is_closed};
    }
    if (is_get_data) {
      return {chosen_index, ch_data, is_get_data};
    }
    // 阻塞50ms后，继续轮训
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return {};
}
}  // namespace PIPELINE

#endif  // PIPELINE_COMM_UTILS_H_
