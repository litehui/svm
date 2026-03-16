//
// Created by litehui on 25-6-18.
//

#ifndef SYSENVUTIL_H
#define SYSENVUTIL_H

#include <string>
#include <map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class SysEnvUtil {
public:
    enum class EnvScope {
        System,
        User
    };

    explicit SysEnvUtil(EnvScope scope);
    ~SysEnvUtil();

    // ========== 原有 wstring 接口（保留向后兼容性）==========
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

    // ========== 新增 UTF-8 (std::string) 接口 ==========
    // 修改原因：项目统一使用 UTF-8 编码，仅在与 Windows API 交互时进行编码转换
    // 使用 StringUtil 进行 UTF-8 ↔ UTF-16 转换

    // 获取所有环境变量键值对（UTF-8 版本）
    std::map<std::string, std::string> getAllUtf8() const;

    // 删除指定环境变量（UTF-8 版本）
    bool removeUtf8(const std::string& key);

    // 检查环境变量是否存在（UTF-8 版本）
    bool existsUtf8(const std::string& key) const;

    // 重命名环境变量键（UTF-8 版本）
    bool renameKeyUtf8(const std::string& oldKey, const std::string& newKey);

    // 设置环境变量值（UTF-8 版本）
    bool setValueUtf8(const std::string& key, const std::string& value);

    // 获取环境变量值（UTF-8 版本）
    std::string getValueUtf8(const std::string& key) const;

    // 设置环境变量并返回旧值（UTF-8 版本）
    std::string setValueAndGetOldUtf8(const std::string& key, const std::string& value);

    // 追加值到环境变量并返回旧值（UTF-8 版本）
    std::string appendValueUtf8(const std::string& key, const std::string& value, const char separator = ';');

private:
    EnvScope m_scope;

#ifdef _WIN32
    HKEY m_rootKey;
    std::wstring m_regPath;

    void broadcastChange() const;
    bool openKey(HKEY* hKey, REGSAM permissions) const;
#endif
};

#endif //SYSENVUTIL_H
