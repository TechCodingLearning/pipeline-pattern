/*
 * @Author: lourisxu
 * @Date: 2024-03-24 20:43:20
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-24 22:26:21
 * @FilePath: /pipeline/comm/rate_limiter.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#ifndef PIPELINE_COMM_RATER_H_
#define PIPELINE_COMM_RATER_H_
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "limiter.h"

namespace PIPELINE {

class RateLimiter : Limiter {
 public:
  RateLimiter();
  long Acquire();
  long Acquire(int permits);

  bool TryAcquire(int timeouts);
  bool TryAcquire(int permits, int timeout);

  double GetRate() const;
  void SetRate(double rate);

 private:
  void sync(unsigned long long now);
  std::chrono::microseconds claim_next(double permits);

 private:
  double interval_;
  double max_permits_;
  double stored_permits_;

  unsigned long long next_free_;

  std::mutex mut_;
};

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_RATER_H_
