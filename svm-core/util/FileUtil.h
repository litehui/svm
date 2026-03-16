//
// Created by litehui on 26-3-16.
// 文件工具类
//

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <filesystem>

class FileUtil {
public:
    // 获取模块路径
    static std::filesystem::path getModulePath();
    
    // 复制目录
    static bool copyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);
};

#endif // FILEUTIL_H
