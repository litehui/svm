//
// Created by litehui on 25-6-18.
//

#ifndef SYSENVUTIL_H
#define SYSENVUTIL_H

#include <windows.h>
#include <string>
#include <map>
#include <vector>

class SysEnvUtil {
public:
    enum class EnvScope {
        System,
        User
    };

    explicit SysEnvUtil(EnvScope scope);
    ~SysEnvUtil();

    // 获取所有环境变量键值对
    std::map<std::wstring, std::wstring> getAll() const;

    // 删除指定环境变量
    bool remove(const std::wstring& key);

    // 检查环境变量是否存在
    bool exists(const std::wstring& key) const;

    // 重命名环境变量键
    bool renameKey(const std::wstring& oldKey, const std::wstring& newKey);

    // 设置环境变量值
    bool setValue(const std::wstring& key, const std::wstring& value);

    // 获取环境变量值
    std::wstring getValue(const std::wstring& key) const;

    // 设置环境变量并返回旧值
    std::wstring setValueAndGetOld(const std::wstring& key, const std::wstring& value);

    // 追加值到环境变量并返回旧值
    std::wstring appendValue(const std::wstring& key, const std::wstring& value, const wchar_t separator = L';');

private:
    EnvScope m_scope;
    HKEY m_rootKey;
    std::wstring m_regPath;

    void broadcastChange() const;
    bool openKey(HKEY* hKey, REGSAM permissions) const;
};

#endif //SYSENVUTIL_H
