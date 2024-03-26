/*
 * @Author: lourisxu
 * @Date: 2024-03-26 07:45:36
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-26 08:05:18
 * @FilePath: /pipeline/debug.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_DEBUG_H_
#define PIPELINE_DEBUG_H_

#include <stdio.h>

#include <string>

namespace PIPELINE {

bool ddddDebug = false;

template <typename... Args>
void DDDDDebug(const std::string &format, const Args &...args) {
  if (ddddDebug) {
    std::printf(("DDDDD " + format).c_str(), args...);
  }
}

void EnableDebug() { ddddDebug = true; }

void DisblaeDebug() { ddddDebug = false; }

}  // namespace PIPELINE

#endif  // PIPELINE_DEBUG_H_
