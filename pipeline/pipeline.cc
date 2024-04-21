/*
 * @Author: lourisxu
 * @Date: 2024-03-23 19:15:58
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 19:38:25
 * @FilePath: /pipeline/pipeline/pipeline.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */
#include "pipeline.h"

#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

#include "comm/debug.h"
#include "comm/functional.h"

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
      throw std::runtime_error("InChanNum of first stage must be 0");
    }

    if (i == n - 1) {
      if (s->OutChanNum() != 0) {
        throw std::runtime_error("OutChanNum of last stage must be 0");
      }
    } else {
      if (s->OutChanNum() != this->stages_[i + 1]->InChanNum()) {
        throw std::runtime_error(pprintf(
            "InChanNum[%d] of stage[%d] must equal OutChanNum[%d] of stage[%d]",
            this->stages_[i + 1]->InChanNum(), i + 1, s->OutChanNum(), i));
      }
    }
  }
}

void Pipeline::Process() {
  DDDDDebug("pipeline start to process..");
  try {
    // 验证规则
    this->Validate();

    // 执行
    int n_thread = this->stages_.size();
    std::vector<std::promise<bool>> result_promises(n_thread);
    std::vector<std::future<bool>> result_futures(n_thread);
    std::vector<std::thread> threads;

    std::vector<BlockingQueue<ChannelData>*> last_outs;
    for (int i = 0; i < n_thread; i++) {
      Stage* s = this->stages_[i];

      // 每个Stage的扇入通道
      std::vector<BlockingQueue<ChannelData>*> tmp_ins;
      for (BlockingQueue<ChannelData>* bq : last_outs) {
        tmp_ins.push_back(bq);
      }

      // 每个Stage的扇出通道
      std::vector<BlockingQueue<ChannelData>*> new_outs;
      for (int j = 0; j < s->OutChanNum(); j++) {
        new_outs.push_back(new BlockingQueue<ChannelData>());
      }

      std::vector<BlockingQueue<ChannelData>*> tmp_outs;
      for (BlockingQueue<ChannelData>* bq : new_outs) {
        tmp_outs.push_back(bq);
      }

      result_futures[i] = result_promises[i].get_future();
      threads.push_back(std::thread(&PIPELINE::Stage::Run, this->stages_[i],
                                    std::move(result_promises[i]), i,
                                    std::move(tmp_ins), std::move(tmp_outs)));

      // 上一个Stage的扇出通道 == 下一个Stage的扇入通道
      last_outs = new_outs;
    }

    // 阻塞式捕获子线程的异常

    for (int i = 0; i < n_thread; i++) {
      result_futures[i].get();  // 子线程如果发生异常，则抛出
    }

    // 阻塞等待所有线程执行完成
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

  } catch (const std::future_error& e) {
    std::cerr << "Run Pipeline FutureError: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  } catch (const std::exception& e) {
    std::cerr << "Run Pipeline Exception caught: " << e.what() << std::endl;
    throw std::runtime_error(e.what());
  } catch (const std::runtime_error& e) {
    throw std::runtime_error("Unkown error");
  }

  DDDDDebug("The process of pipeline is complete.");
}

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
    lines[0] += pprintf(maxLens[i], format, s->name_.c_str());
    lines[0] += "    ";
    lines[1] +=
        pprintf(maxLens[i], format, std::string(maxLens[i], '-').c_str());
    lines[1] += "    ";
    int j = 2;
    for (auto& h : s->handlers_) {
      for (int k = 0; k < std::max(h->InChanNum(), h->OutChanNum()); k++) {
        if (k == 0) {
          lines[j] += pprintf(maxLens[i], format, h->Name().c_str());
        } else {
          lines[j] += pprintf(maxLens[i], format,
                              std::string(h->Name().length(), '*').c_str());
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
      lines[j] +=
          pprintf(maxLens[i], format, std::string(maxLens[i], ' ').c_str());
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
