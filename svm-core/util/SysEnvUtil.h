//
// Created by litehui on 25-5-23.
//

#ifndef SYSENVUTIL_H
#define SYSENVUTIL_H

// SysEnvUtil.h

#include <string>
#include <vector>
#include <optional>
#include <windows.h>

enum class EnvType {
    System,
    User
};

class SysEnvUtil {
public:
    explicit SysEnvUtil(EnvType type);

    // 查询所有键值对
    std::vector<std::pair<std::wstring, std::wstring>> getAllKeys() const;

    // 删除指定的 key（如果存在）
    bool deleteKey(const std::wstring& key) const;

    // 检查给定 key 是否存在
    bool containsKey(const std::wstring& key) const;

    // 修改 key 的名称（重命名）
    bool renameKey(const std::wstring& oldKey, const std::wstring& newKey) const;

    // 设置指定 key 的值
    bool setKeyValue(const std::wstring& key, const std::wstring& value) const;

    // 获取指定 key 的值
    std::optional<std::wstring> getKeyValue(const std::wstring& key) const;

    // 设置指定 key 的值，并返回旧值
    std::optional<std::wstring> setKeyValueAndGetOld(const std::wstring& key, const std::wstring& value) const;

    // 将 value 追加到 key 的当前值，并返回旧值
    std::optional<std::wstring> appendToValue(const std::wstring& key, const std::wstring& value) const;

private:
    EnvType envType_;
    HKEY getRootKey() const;
};


#endif //SYSENVUTIL_H
