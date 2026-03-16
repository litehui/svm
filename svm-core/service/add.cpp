//
// Created by litehui on 26-3-16.
// 添加 SDK 或 SDK 版本功能实现
//

#include "add.h"
#include "../util/ConfigUtil.h"
#include <filesystem>
#include <iostream>
#include <windows.h>

// 使用 FileUtil 类
#include "../util/FileUtil.h"

const std::filesystem::path MODULE_ROOT = FileUtil::getModulePath();

// 原常量改为动态路径
const std::filesystem::path DEFAULT_REPOSITORY_BASE_PATH = MODULE_ROOT / "repository";

// 使用 FileUtil::copyDirectory

int add(const char* sdk, const char* version, const char* sourcePath) {
    try {
        ConfigUtil configUtil("../config/application.properties");

        // 构建路径
        std::filesystem::path repositoryPath = 
            (!std::string{configUtil.getProperty("repositoryBasePath")}.empty() ?
                std::filesystem::path(configUtil.getProperty("repositoryBasePath")) :
                DEFAULT_REPOSITORY_BASE_PATH) / sdk / version;

        // 检查 repository 根目录是否存在，不存在则创建
        std::filesystem::path repoRoot = repositoryPath.parent_path().parent_path();
        if (!std::filesystem::exists(repoRoot)) {
            std::filesystem::create_directories(repoRoot);
        }

        // 检查 SDK 目录是否存在，不存在则创建
        std::filesystem::path sdkPath = repositoryPath.parent_path();
        if (!std::filesystem::exists(sdkPath)) {
            std::filesystem::create_directories(sdkPath);
            std::wcout << L"Created SDK directory: " << sdkPath.wstring() << std::endl;
        }

        // 检查版本目录是否存在，不存在则创建
        if (!std::filesystem::exists(repositoryPath)) {
            std::filesystem::create_directories(repositoryPath);
            std::wcout << L"Created version directory: " << repositoryPath.wstring() << std::endl;
        } else {
            std::wcout << L"Version directory already exists: " << repositoryPath.wstring() << std::endl;
        }

        // 如果提供了源路径，复制文件到版本目录
        if (sourcePath && *sourcePath) {
            std::filesystem::path source(sourcePath);
            if (std::filesystem::exists(source)) {
                std::wcout << L"Copying files from: " << source.wstring() << std::endl;
                std::wcout << L"Copying files to: " << repositoryPath.wstring() << std::endl;
                if (!FileUtil::copyDirectory(source, repositoryPath)) {
                    return -1;
                }
                std::wcout << L"Files copied successfully" << std::endl;
            } else {
                std::wcerr << L"Source path does not exist: " << source.wstring() << std::endl;
                return -1;
            }
        }

        std::wcout << L"Successfully added SDK: " << sdk << L" version: " << version << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::wcerr << L"Config load failed: " << e.what() << std::endl;
        return -1;
    }
}
