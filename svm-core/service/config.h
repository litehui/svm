//
// Created by litehui on 26-3-16.
// 配置管理功能
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// 配置管理功能
// @param key 配置键名 (repositoryBasePath 或 linkBasePath)
// @param value 配置值 (新的路径)
// @return 0 成功，-1 失败
int config(const char* key, const char* value);

#endif // CONFIG_H
