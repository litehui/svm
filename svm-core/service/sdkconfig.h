//
// Created by litehui on 26-3-16.
// SDK 环境变量配置管理功能
//

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

#include <string>
#include <vector>
#include <map>

// SDK 配置结构
struct SdkConfig {
    std::string homeEnv;         // HOME 环境变量名 (如 JAVA_HOME)
    std::vector<std::string> pathFolders; // 需要添加到 Path 的文件夹
};

// 设置 SDK 环境变量配置
// @param sdk SDK 名称
// @param homeEnv HOME 环境变量名
// @param pathFolders 需要添加到 Path 的文件夹列表
// @return 0 成功，-1 失败
int sdkconfig(const char* sdk, const char* homeEnv, const char* pathFolders);

// 显示 SDK 环境变量配置
// @param sdk SDK 名称（可选，为空时显示所有配置）
// @return 0 成功，-1 失败
int sdkconfig_show(const char* sdk = nullptr);

// 内部函数：获取 SDK 配置
std::map<std::string, SdkConfig> getSdkConfigs();

#endif // SDKCONFIG_H
