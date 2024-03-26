/*
 * @Author: lourisxu
 * @Date: 2024-03-24 22:25:22
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-24 22:26:11
 * @FilePath: /pipeline/comm/limiter.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_COMM_LIMITER_H_
#define PIPELINE_COMM_LIMITER_H_

namespace PIPELINE {

class Limiter {
 public:
  virtual long Acquire() = 0;
};

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_LIMITER_H_
