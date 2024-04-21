'''
Author: lourisxu
Date: 2024-03-23 19:59:09
LastEditors: lourisxu
LastEditTime: 2024-04-21 19:31:11
FilePath: /pipeline/BUILD
Description: 

Copyright (c) 2024 by lourisxu, All Rights Reserved. 
'''
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

all_srcs = glob(["comm/*.cc", "pipeline/*.cc"])
all_hdrs = glob(["comm/*.h", "pipeline/*.h"])
include_dir = ["comm", "pipeline"]

cc_library(
	name = "pipeline",
	srcs = all_srcs,
	hdrs = all_hdrs,
    visibility = ["//visibility:public"],
    includes = include_dir,
	# copts = ["-std=c++17"],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":pipeline",
    ],
)

cc_test(
	name ="component_test",
	srcs = glob(["example/component_test.cc", "comm/*.cc", "comm/*.h"]),
	includes = ["example", "comm"],
	deps = [
		"@com_google_googletest//:gtest_main",
		"@com_google_googletest//:gtest_sample_lib",
	],
)

cc_test(
	name ="pipeline_test",
	srcs = ["example/pipeline_test.cc"],
	includes = ["example"],
	deps = [
		":pipeline",
		"@com_google_googletest//:gtest_main",
		"@com_google_googletest//:gtest_sample_lib",
	],
)