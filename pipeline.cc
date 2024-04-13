/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:15:58
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-14 00:19:31
 * @FilePath: /pipeline/pipeline.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include "pipeline.h"

#include "functional.h"

namespace PIPELINE {

Pipeline* Pipeline::AddStage(Stage* stage) {
  this->stages_.push_back(stage);
  return this;
}

template <typename... HandlerPtr>
Pipeline* Pipeline::AddNewStage(std::string name, HandlerPtr... handlers) {
  this->AddStage(new Stage(name, {handlers...}));
  return this;
}

void Pipeline::Validate() {
  int n = this->stages_.size();
  for (int i = 0; i < n; i++) {
    Stage* s = this->stages_[i];
    if (i == 0 && s->InChanNum() != 0) {
      std::runtime_error("InChanNum of first stage must be 0");
    }

    if (i == n - 1) {
      if (s->OutChanNum() != 0) {
        std::runtime_error(pprintf(
            "InChanNum[%d] of stage[%d] must equal OutChanNum[%d] of stage[%d]",
            this->stages_[i + 1]->InChanNum(), i + 1, s->OutChanNum(), i));
      }
    }
  }
}

void Pipeline::Process() {}

std::string Pipeline::String() {
  // 计算每个Stage打印需要的列数和行数
  std::vector<int> maxLens(this->stages_.size());
  int lineCount = 0;
  int n = this->stages_.size();
  for (int i = 0; i < n; i++) {
    Stage* s = this->stages_[i];
    maxLens[i] = s->name_.length();

    int cnt = 0;
    for (auto& h : s->handlers_) {
      if (h->Name().length() > maxLens[i]) {
        maxLens[i] = h->Name().length();
      }

      cnt += std::max(h->InChanNum(), h->OutChanNum());
    }

    if (cnt > lineCount) {
      lineCount = cnt;
    }
  }

  // 根据每个Stage的所有handler计算每行打印的内容
  std::vector<std::string> lines(lineCount + 2);
  for (int i = 0; i < n; i++) {
    Stage* s = this->stages_[i];
    std::string format = "| %s |";
    lines[0] += pprintf(maxLens[i], format, s->name_);
    lines[0] += "    ";
    lines[1] += pprintf(maxLens[i], format, std::string(maxLens[i], '-'));
    lines[1] += "    ";
    int j = 2;
    for (auto& h : s->handlers_) {
      for (int k = 0; k < std::max(h->InChanNum(), h->OutChanNum()); k++) {
        if (k == 0) {
          lines[j] += pprintf(maxLens[i], format, h->Name());
        } else {
          lines[j] +=
              pprintf(maxLens[i], format, std::string(h->Name().length(), '*'));
        }

        // 输出通道打印->
        if (k < h->OutChanNum()) {
          lines[j] += " -> ";
        } else {
          lines[j] += "    ";
        }

        j++;
      }
    }

    // 若某个Stage的行数不足需要填充
    for (; j < lineCount + 2; j++) {
      lines[j] += pprintf(maxLens[i], format, std::string(maxLens[i], ' '));
      lines[j] += "    ";
    }
  }

  // 输出
  std::string res;
  res += std::string(lines[0].length(), '-');
  res += "\n";
  for (int i = 0; i < lineCount + 2; i++) {
    res += lines[i] + "\n";
  }
  res += std::string(lines[0].length(), '-');
  return res;
}
}  // namespace PIPELINE
