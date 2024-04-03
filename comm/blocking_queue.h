/*
 * @Author: lourisxu
 * @Date: 2024-03-24 19:27:55
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-31 08:56:52
 * @FilePath: /pipeline/comm/blocking_queue.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_BLOCKING_QUEUE_H_
#define PIPELINE_COMM_BLOCKING_QUEUE_H_

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

namespace PIPELINE {

const unsigned int MAX_QUE_COUNT = 65535;  // 阻塞队列最大元素数

// 阻塞队列
template <typename T>
class BlockingQueue {
 public:
  BlockingQueue() : is_closed_(false) {}
  void Push(T item) {
    {
      std::lock_guard<std::mutex> lock(this->mtx_);
      if (this->is_closed_) {
        throw std::runtime_error(
            "blocking queue has been closed! Cannot push element again!");
      }
      if (this->queue_.size() >= MAX_QUE_COUNT) {
        return;
      }
      this->queue_.push(std::move(item));
    }
    this->cond_.notify_one();
  }

  bool TryAndPop(T& popped_item) {
    std::lock_guard<std::mutex> lock(this->mtx_);
    if (this->queue_.empty()) {
      return false;
    }
    popped_item = std::move(this->queue_.front());
    this->queue_.pop();
    return true;
  }

  void WaitAndPop(T& popped_item) {
    std::unique_lock<std::mutex> lock(this->mtx_);
    while (this->queue_.empty()) {
      this->cond_.wait(lock);
    }
    popped_item = std::move(this->queue_.front());
    this->queue_.pop();
  }

  bool Empty() const {
    std::lock_guard<std::mutex> lock(this->mtx_);
    return this->queue_.empty();
  }

  unsigned int Size() const {
    std::lock_guard<std::mutex> lock(this->mtx_);
    return this->queue_.size();
  }

  void Close() {
    std::lock_guard<std::mutex> lock(this->mtx_);
    this->is_closed_ = true;
  }

  bool Closed() {
    std::lock_guard<std::mutex> lock(this->mtx_);
    return this->is_closed_;
  }

 private:
  std::queue<T> queue_;
  mutable std::mutex mtx_;
  std::condition_variable cond_;
  bool is_closed_;

  BlockingQueue& operator=(const BlockingQueue&) = delete;  // 禁用赋值构造函数
  BlockingQueue(const BlockingQueue& other) = delete;  // 禁用构造函数
};

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_BLOCKING_QUEUE_H_