/*
 * @Author: lourisxu
 * @Date: 2024-04-15 09:49:39
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 16:36:33
 * @FilePath: /pipeline/comm/utils.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include <algorithm>
#include <iostream>
#include <random>
#include <thread>
#include <tuple>
#include <vector>

#include "blocking_queue.h"
#include "defines.h"
#include "util.h"

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
    std::cout << "ch close" << std::endl;
    return {chosen_index, data, false, true};
  }

  bool ok = ch->TryAndPop(data);
  return {chosen_index, data, ok, false};
}

std::tuple<int, ChannelData, bool> Select(
    const std::string &handle_name,
    std::vector<BlockingQueue<ChannelData> *> &ins,
    const std::chrono::milliseconds &timeout) {
  auto startTime = std::chrono::steady_clock::now();
  while (true) {
    auto [chosen_index, ch_data, is_get_data, is_closed] = SelectByRandom(ins);

    if (is_closed) {
      std::cout << "select is close" << std::endl;
      return {chosen_index, ch_data, false};
    }
    if (is_get_data) {
      if (ch_data.data != nullptr) {
        std::cout << handle_name
                  << "=== in Select data: [index: " << ch_data.index
                  << " data: " << *(static_cast<int *>(ch_data.data)) << "]"
                  << std::endl;
      } else {
        std::cout << handle_name << "=== in data: [index: " << ch_data.index
                  << " data: " << ch_data.data << "]" << std::endl;
      }
      return {chosen_index, ch_data, true};
    }
    // 阻塞50ms后，继续轮训
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 计算已经运行的时间
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - startTime);
    if (elapsedTime >= timeout) {
      throw std::runtime_error("Acquiring data timeout from blocking queue!");
    }
  }
  return {};
}

}  // namespace PIPELINE
