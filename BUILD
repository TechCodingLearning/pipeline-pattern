'''
Author: lourisxu
Date: 2024-03-23 19:59:09
LastEditors: lourisxu
LastEditTime: 2024-03-29 10:44:40
FilePath: /pipeline/BUILD
Description: 

Copyright (c) 2024 by lourisxu, All Rights Reserved. 
'''
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")

all_srcs = glob(["comm/*.cc", "comm/*.h", "*.cc", "*.h"])
include_dir = ["comm"]

cc_binary(
	name="pipeline",
	srcs=all_srcs,
    visibility = ["//visibility:public"],
    includes = include_dir,
	# copts = ["-std=c++17"],
)

cc_test(
	name ="component_test",
	srcs = glob(["example/*.cc", "example/*.h", "comm/*.cc", "comm/*.h"]),
	includes=["example"],
	deps=[
		":pipeline",
		"@com_google_googletest//:gtest_main",
		"@com_google_googletest//:gtest_sample_lib",
	],
)