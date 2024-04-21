/*
 * @Author: lourisxu
 * @Date: 2024-04-14 23:22:04
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 18:18:22
 * @FilePath: /pipeline/comm/debug.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_DEBUG_H_
#define PIPELINE_COMM_DEBUG_H_

#include <iostream>
#include <string>

namespace PIPELINE {

extern bool ddddDebug;

template <typename... Args>
void DDDDDebug(const std::string &format, const Args &...args) {
  if (ddddDebug) {
    std::printf(("DDDDD " + format + "\n\n").c_str(), args...);
  }
}

void EnableDebug();

void DisblaeDebug();

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_DEBUG_H_
