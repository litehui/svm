//
// Created by litehui on 26-3-16.
// 字符串编码转换工具类实现
//

#include "StringUtil.h"
#include <windows.h>
#include <stdexcept>

std::wstring StringUtil::utf8_to_utf16(const std::string& utf8Str) {
    // 如果输入字符串为空，返回空字符串
    if (utf8Str.empty()) {
        return std::wstring();
    }

    // 第一步：计算需要的缓冲区大小
    int length = MultiByteToWideChar(
        CP_UTF8,              // 源字符串编码格式为 UTF-8
        0,                     // 标志位，0 表示默认行为
        utf8Str.c_str(),       // 源字符串
        static_cast<int>(utf8Str.size()),  // 源字符串长度
        nullptr,                // 目标缓冲区，传入 nullptr 获取所需长度
        0                       // 目标缓冲区大小，0 表示只计算长度
    );

    if (length == 0) {
        throw std::runtime_error("Failed to calculate buffer size for UTF-8 to UTF-16 conversion");
    }

    // 第二步：创建足够大的缓冲区并进行转换
    std::wstring utf16Str(length, L'\0');
    int result = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8Str.c_str(),
        static_cast<int>(utf8Str.size()),
        &utf16Str[0],
        length
    );

    if (result == 0) {
        throw std::runtime_error("Failed to convert UTF-8 to UTF-16");
    }

    return utf16Str;
}

std::string StringUtil::utf16_to_utf8(const std::wstring& utf16Str) {
    // 如果输入字符串为空，返回空字符串
    if (utf16Str.empty()) {
        return std::string();
    }

    // 第一步：计算需要的缓冲区大小
    int length = WideCharToMultiByte(
        CP_UTF8,              // 目标字符串编码格式为 UTF-8
        0,                     // 标志位，0 表示默认行为
        utf16Str.c_str(),      // 源字符串
        static_cast<int>(utf16Str.size()),  // 源字符串长度
        nullptr,                // 目标缓冲区，传入 nullptr 获取所需长度
        0,                       // 目标缓冲区大小，0 表示只计算长度
        nullptr,                 // 默认字符，nullptr 表示使用系统默认
        nullptr                  // 标志位指针
    );

    if (length == 0) {
        throw std::runtime_error("Failed to calculate buffer size for UTF-16 to UTF-8 conversion");
    }

    // 第二步：创建足够大的缓冲区并进行转换
    std::string utf8Str(length, '\0');
    int result = WideCharToMultiByte(
        CP_UTF8,
        0,
        utf16Str.c_str(),
        static_cast<int>(utf16Str.size()),
        &utf8Str[0],
        length,
        nullptr,
        nullptr
    );

    if (result == 0) {
        throw std::runtime_error("Failed to convert UTF-16 to UTF-8");
    }

    return utf8Str;
}
