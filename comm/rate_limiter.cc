/*
 * @Author: lourisxu
 * @Date: 2024-03-24 21:10:51
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-24 22:24:04
 * @FilePath: /pipeline/comm/rate_limiter.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "rate_limiter.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

namespace PIPELINE {

RateLimiter::RateLimiter()
    : interval_(0), max_permits_(0), stored_permits_(0), next_free_(0) {}

long RateLimiter::Acquire() { return Acquire(1); }

long RateLimiter::Acquire(int permits) {
  if (permits <= 0) {
    std::runtime_error("RateLimiter: Must request positive amount of permits");
  }

  auto wait_time = claim_next(permits);
  std::this_thread::sleep_for(wait_time);

  return wait_time.count() / 1000.0;
}

bool RateLimiter::TryAcquire(int permits) { return TryAcquire(permits, 0); }

bool RateLimiter::TryAcquire(int permits, int timeout) {
  using namespace std::chrono;

  unsigned long long now =
      duration_cast<microseconds>(system_clock::now().time_since_epoch())
          .count();

  // Check to see if the next free aquire time is within the
  // specified timeout. If it's not, return false and DO NOT BLOCK,
  // otherwise, calculate time needed to claim, and block
  if (next_free_ > now + timeout * 1000)
    return false;
  else {
    Acquire(permits);
  }

  return true;
}

void RateLimiter::sync(unsigned long long now) {
  // If we're passed the next_free, then recalculate
  // stored permits, and update next_free_
  if (now > next_free_) {
    stored_permits_ = std::min(
        max_permits_, stored_permits_ + (now - next_free_) / interval_);
    next_free_ = now;
  }
}
std::chrono::microseconds RateLimiter::claim_next(double permits) {
  using namespace std::chrono;

  std::lock_guard<std::mutex> lock(mut_);

  unsigned long long now =
      duration_cast<microseconds>(system_clock::now().time_since_epoch())
          .count();

  // Make sure we're synced
  sync(now);

  // Since we synced before hand, this will always be >= 0.
  unsigned long long wait = next_free_ - now;

  // Determine how many stored and freh permits to consume
  double stored = std::min(permits, stored_permits_);
  double fresh = permits - stored;

  // In the general RateLimiter, stored permits have no wait time,
  // and thus we only have to wait for however many fresh permits we consume
  long next_free = (long)(fresh * interval_);

  next_free_ += next_free;
  stored_permits_ -= stored;

  return microseconds(wait);
}

double RateLimiter::GetRate() const { return 1000000.0 / interval_; }
void RateLimiter::SetRate(double rate) {
  if (rate <= 0.0) {
    throw std::runtime_error("RateLimiter: Rate must be greater than 0");
  }

  std::lock_guard<std::mutex> lock(mut_);
  interval_ = 1000000.0 / rate;
}

}  // namespace PIPELINE
