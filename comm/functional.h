/*
 * @Author: lourisxu
 * @Date: 2024-03-24 19:53:39
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-13 21:33:39
 * @FilePath: /pipeline/comm/functional.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_FUNCTIONAL_H_
#define PIPELINE_COMM_FUNCTIONAL_H_

#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

namespace PIPELINE {

const int MAX_BUF_SIZE = 512;

struct AddOneFunc {
  int operator()(int x) const { return x + 1; }

  std::vector<int> operator()(std::vector<int>& x) const {
    std::for_each(std::begin(x), std::end(x), [](int& ele) { ele += 1; });
    return x;
  }
};

template <class T>
class PipeClosue : public T {
 public:
  template <class... Xs>
  PipeClosue(Xs&&... xs) : T(std::forward<Xs>(xs)...) {}
};

template <typename L, typename F>
auto operator|(L&& x, const PipeClosue<F> p) {
  return p(x);
}

template <typename... Args>
std::string pprintf(const std::string& format, const Args&... args) {
  char buffer[MAX_BUF_SIZE];
  std::snprintf(buffer, sizeof(buffer), format.c_str(), args...);
  return std::string(buffer);
}

template <typename Container>
std::string pprintf(const Container& container) {
  std::ostringstream oss;
  oss << "[ ";
  for (auto it = container.begin(); it != container.end(); it++) {
    oss << *it;
    oss << " ";
  }
  oss << "]";
  return oss.str();
}

template <typename Key, typename Value>
std::string pprintf(const std::map<Key, Value>& mp) {
  std::ostringstream oss;
  oss << "[ ";
  for (const auto& pair : mp) {
    oss << pair.first << ": " << pair.second;
    oss << " ";
  }
  oss << "]";
  return oss.str();
}

// 固定列宽格式化串
template <typename... Args>
std::string pprintf(const int& width, const std::string& format,
                    const Args&... args) {
  char buffer[MAX_BUF_SIZE];
  std::snprintf(buffer, sizeof(buffer), format.c_str(), args...);

  std::ostringstream oss;
  oss << std::left << std::setw(width) << std::string(buffer);
  return oss.str();
}

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_FUNCTIONAL_H_
