'''
Author: lourisxu
Date: 2024-03-23 19:59:09
LastEditors: lourisxu
LastEditTime: 2024-03-24 20:19:44
FilePath: /pipeline/BUILD
Description: 

Copyright (c) 2024 by lourisxu, All Rights Reserved. 
'''
load("@rules_cc//cc:defs.bzl", "cc_binary")

all_srcs = glob(["comm/*.cc", "comm/*.h", "*.cc", "*.h"])
include_dir = ["comm"]

cc_binary(
	name="pipeline",
	srcs=all_srcs,
    visibility = ["//visibility:public"],
    includes = include_dir,
	copts = ["-std=c++17"],
)
