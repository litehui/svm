//
// Created by litehui on 26-3-16.
// 字符串编码转换工具类
// 用于 UTF-8 (std::string) 和 UTF-16 (std::wstring) 之间的转换
//

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>

class StringUtil {
public:
    // 将 UTF-8 编码的 std::string 转换为 UTF-16 编码的 std::wstring
    static std::wstring utf8_to_utf16(const std::string& utf8Str);
    
    // 将 UTF-16 编码的 std::wstring 转换为 UTF-8 编码的 std::string
    static std::string utf16_to_utf8(const std::wstring& utf16Str);
};

#endif // STRINGUTIL_H
