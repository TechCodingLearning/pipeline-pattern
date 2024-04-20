/*
 * @Author: lourisxu
 * @Date: 2024-03-26 07:45:36
 * @LastEditors: lourisxu
 * @LastEditTime: 2024-04-15 17:06:51
 * @FilePath: /pipeline/comm/debug.cc
 * @Description:
 *
 * Copyright (c) 2024 by lourisxu, All Rights Reserved.
 */

#include "debug.h"

#include <stdio.h>

namespace PIPELINE {

bool ddddDebug = false;

void EnableDebug() { ddddDebug = true; }

void DisblaeDebug() { ddddDebug = false; }

}  // namespace PIPELINE
