/*
 * @Author: lourisxu
 * @Date: 2024-03-24 19:53:39
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-03-24 20:30:57
 * @FilePath: /pipeline/comm/functional.h
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#ifndef PIPELINE_COMM_FUNCTIONAL_H_
#define PIPELINE_COMM_FUNCTIONAL_H_

#include <numeric>
#include <vector>

namespace PIPELINE {

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

}  // namespace PIPELINE

#endif  // PIPELINE_COMM_FUNCTIONAL_H_
