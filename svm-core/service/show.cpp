//
// Created by litehui on 25-5-22.
//

#include "show.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../util/ConfigUtil.h"
#include "../util/SysEnvUtil.h"

// 类型别名定义
using DirectoryPair = std::pair<std::string, std::string>;
using DirectoryList = std::vector<DirectoryPair>;

// 检查 SDK 是否正在被系统环境变量使用
// @param sdk SDK 名称
// @param version SDK 版本
// @return true 如果正在使用，false 否则
bool isSdkVersionInUse(const std::string& sdk, const std::string& version) {
    SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
    SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
    
    std::string sdkUpper = sdk;
    for (char& c : sdkUpper) {
        c = std::toupper(c);
    }
    
    std::string homeEnv = sdkUpper + "_HOME";
    std::wstring wHomeEnv = std::wstring(homeEnv.begin(), homeEnv.end());
    
    std::wstring sysValue = sysEnvUtil.getValue(wHomeEnv);
    std::wstring userValue = userEnvUtil.getValue(wHomeEnv);
    
    // 检查系统环境变量
    if (!sysValue.empty()) {
        std::filesystem::path sysPath(sysValue);
        if (sysPath.filename().string() == version) {
            return true;
        }
    }
    
    // 检查用户环境变量
    if (!userValue.empty()) {
        std::filesystem::path userPath(userValue);
        if (userPath.filename().string() == version) {
            return true;
        }
    }
    
    return false;
}

constexpr std::wstring_view DEFAULT_REPOSITORY_BASE_PATH = L"..\\repository";

// 目录扫描函数
DirectoryList scanDirectories(const std::filesystem::path& rootDir) {
    DirectoryList result;
    for (const auto& entry : std::filesystem::directory_iterator(rootDir)) {
        if (entry.is_directory()) {
            std::string firstLevel = entry.path().filename().string();
            for (const auto& subEntry : std::filesystem::directory_iterator(entry.path())) {
                if (subEntry.is_directory()) {
                    std::string secondLevel = subEntry.path().filename().string();
                    result.emplace_back(firstLevel, secondLevel);
                }
            }
        }
    }
    return result;
}

// 结果显示函数
void displayDirectories(const DirectoryList& directories) {
    for (const auto& [first, second] : directories) {
        bool inUse = isSdkVersionInUse(first, second);
        if (inUse) {
            std::cout << first << "\t" << second << "\t[IN USE]" << std::endl;
        } else {
            std::cout << first << "\t" << second << std::endl;
        }
    }
}

// 主函数
std::string show() {
    ConfigUtil configUtil("../config/application.properties");
    std::filesystem::path repositoryPath =
        (!std::string{configUtil.getProperty("repositoryBasePath")}.empty() ?
         std::filesystem::path(configUtil.getProperty("repositoryBasePath")) :
         DEFAULT_REPOSITORY_BASE_PATH);

    // 检查路径有效性
    if (!std::filesystem::exists(repositoryPath)) {
        std::wcerr << L"repositoryPath not exists" << std::endl;
        return "repositoryPath not exists";
    }

    // 执行目录扫描
    auto directories = scanDirectories(repositoryPath);

    // 处理空结果情况
    if (directories.empty()) {
        std::wcerr << L"No directories found" << std::endl;
        return "No directories found";
    }

    // 显示扫描结果
    displayDirectories(directories);
    return "Success";
}
