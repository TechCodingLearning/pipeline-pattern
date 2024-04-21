<!--
 * @Author: lourisxu
 * @Date: 2024-03-23 20:19:38
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-21 19:44:25
 * @FilePath: /pipeline/README.md
 * @Description: 
 * 
 * Copyright (c) 2024 by lourisxu, All Rights Reserved. 
-->
---

## 执行单元测试用例
```
bazel test --cxxopt=-std=c++17 --test_output=all //:component_test --verbose_failures --sandbox_debug > component_test.log

bazel test --cxxopt=-std=c++17 --test_output=all //:pipeline_test --verbose_failures --sandbox_debug > pipeling_test.log
```

## 编译
```
bazel build //:main --sandbox_debug --cxxopt=-std=c++17
```

## 执行主函数
```
./bazel-bin/main
```


---

# Reference

[1] Go Concurrency Patterns: Pipelines and cancellation: https://go.dev/blog/pipelines

[2] C++ Pipeline Pattern: https://github.com/Saahitya/pipeline_design_pattern

[3] Go CheckComm: https://git.woa.com/prd_base_pay_go/check_comm.git