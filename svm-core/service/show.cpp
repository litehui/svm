//
// Created by litehui on 25-5-22.
//

#include "show.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "../util/ConfigUtil.h"

// 类型别名定义
using DirectoryPair = std::pair<std::string, std::string>;
using DirectoryList = std::vector<DirectoryPair>;

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
        std::cout << first << "\t" << second << std::endl;
    }
}

// 主函数
std::string show() {
    ConfigUtil configUtil("..\\config\\application.properties");
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
