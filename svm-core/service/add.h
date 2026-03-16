//
// Created by litehui on 26-3-16.
// 添加 SDK 或 SDK 版本功能
//

#ifndef ADD_H
#define ADD_H

#include <string>

// 添加 SDK 或 SDK 版本
// @param sdk SDK 名称
// @param version SDK 版本
// @param sourcePath 源路径（可选，默认为空）
// @return 0 成功，-1 失败
int add(const char* sdk, const char* version, const char* sourcePath = nullptr);

#endif // ADD_H
