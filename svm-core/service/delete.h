//
// Created by litehui on 26-3-16.
// 删除 SDK 或 SDK 版本功能
//

#ifndef DELETE_H
#define DELETE_H

#include <string>

// 删除 SDK 或 SDK 版本
// @param sdk SDK 名称
// @param version SDK 版本（可选，为空时删除整个 SDK）
// @return 0 成功，-1 失败
int removeSdk(const char* sdk, const char* version = nullptr);

#endif // DELETE_H
