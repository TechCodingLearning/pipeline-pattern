<!--
 * @Author: lourisxu
 * @Date: 2024-03-23 20:19:38
 * @LastEditors: lourisxu
 * @LastEditTime: 2025-02-08 10:19:00
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

## 运行可执行文件

```
./bin/main
./bin/component_test
./bin/pipeline_test
```

---

# Reference

[1] Go Concurrency Patterns: Pipelines and cancellation: https://go.dev/blog/pipelines

[2] C++ Pipeline Pattern: https://github.com/Saahitya/pipeline_design_pattern
