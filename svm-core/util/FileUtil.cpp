//
// Created by litehui on 26-3-16.
// 文件工具类实现
//

#include "FileUtil.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#include <string>
#endif

std::filesystem::path FileUtil::getModulePath() {
#ifdef _WIN32
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().parent_path(); // 获取项目根目录
#else
    // Linux 平台实现
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path().parent_path();
    }
    // 如果获取失败，返回当前工作目录
    return std::filesystem::current_path();
#endif
}

bool FileUtil::copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination) {
    try {
        if (!std::filesystem::exists(source)) {
            return true; // 源目录不存在，不需要复制
        }

        if (!std::filesystem::exists(destination)) {
            std::filesystem::create_directories(destination);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
            auto destPath = destination / entry.path().lexically_relative(source);
            if (entry.is_directory()) {
                std::filesystem::create_directories(destPath);
            } else {
                std::filesystem::copy(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
        return true;
    } catch (const std::exception& e) {
#ifdef _WIN32
        std::wcerr << L"Failed to copy directory: " << e.what() << std::endl;
#else
        std::cerr << "Failed to copy directory: " << e.what() << std::endl;
#endif
        return false;
    }
}
