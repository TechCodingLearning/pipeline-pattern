/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:48:58
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-20 15:36:21
 * @FilePath: /pipeline/comm/utils.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_UTILS_H_
#define PIPELINE_COMM_UTILS_H_

#include <tuple>
#include <vector>

#include "blocking_queue.h"
#include "defines.h"

namespace PIPELINE {

std::tuple<int, ChannelData, bool, bool> SelectByRandom(
    std::vector<BlockingQueue<ChannelData> *> &ins);

std::tuple<int, ChannelData, bool> Select(
    std::vector<BlockingQueue<ChannelData> *> &ins,
    const std::chrono::milliseconds &timeout);

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_UTILS_H_
