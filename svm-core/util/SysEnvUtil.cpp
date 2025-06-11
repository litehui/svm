// SysEnvUtil.cpp
#include "SysEnvUtil.h"
#include <windows.h>
#include <optional>
#include <vector>
#include <stdexcept>

SysEnvUtil::SysEnvUtil(EnvType type) : envType_(type) {}

HKEY SysEnvUtil::getRootKey() const {
    return (envType_ == EnvType::System) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
}

std::vector<std::pair<std::wstring, std::wstring>> SysEnvUtil::getAllKeys() const {
    HKEY hKey = nullptr;
    auto rootKey = getRootKey();
    LONG result = RegOpenKeyExW(rootKey, L"Environment", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        throw std::runtime_error("Failed to open registry key.");
    }

    std::vector<std::pair<std::wstring, std::wstring>> variables;

    DWORD index = 0;
    WCHAR name[1024];
    DWORD nameSize = sizeof(name) / sizeof(name[0]);
    BYTE data[16384];
    DWORD dataSize = sizeof(data);
    while (true) {
        nameSize = sizeof(name) / sizeof(name[0]);
        dataSize = sizeof(data);
        DWORD type = 0;
        result = RegEnumValueW(hKey, index++, name, &nameSize, nullptr, &type, data, &dataSize);
        if (result == ERROR_NO_MORE_ITEMS) break;
        if (result != ERROR_SUCCESS) continue;

        if (type == REG_SZ || type == REG_EXPAND_SZ) {
            variables.emplace_back(std::wstring(name), std::wstring(reinterpret_cast<WCHAR*>(data)));
        }
    }

    RegCloseKey(hKey);
    return variables;
}

bool SysEnvUtil::deleteKey(const std::wstring& key) const {
    HKEY hKey = nullptr;
    auto rootKey = getRootKey();
    LONG result = RegOpenKeyExW(rootKey, L"Environment", 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) return false;

    result = RegDeleteValueW(hKey, key.c_str());
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool SysEnvUtil::containsKey(const std::wstring& key) const {
    HKEY hKey = nullptr;
    auto rootKey = getRootKey();
    LONG result = RegOpenKeyExW(rootKey, L"Environment", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) return false;

    DWORD type = 0, size = 0;
    result = RegQueryValueExW(hKey, key.c_str(), nullptr, &type, nullptr, &size);
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool SysEnvUtil::renameKey(const std::wstring& oldKey, const std::wstring& newKey) const {
    if (!containsKey(oldKey)) return false;
    if (containsKey(newKey)) return false;

    auto oldValue = getKeyValue(oldKey);
    if (!oldValue.has_value()) return false;

    if (!setKeyValue(newKey, oldValue.value())) return false;
    if (!deleteKey(oldKey)) return false;

    return true;
}

bool SysEnvUtil::setKeyValue(const std::wstring& key, const std::wstring& value) const {
    HKEY hKey = nullptr;
    auto rootKey = getRootKey();
    LONG result = RegOpenKeyExW(rootKey, L"Environment", 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) return false;

    result = RegSetValueExW(hKey, key.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), static_cast<DWORD>((value.size() + 1) * sizeof(WCHAR)));
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

std::optional<std::wstring> SysEnvUtil::getKeyValue(const std::wstring& key) const {
    HKEY hKey = nullptr;
    auto rootKey = getRootKey();
    LONG result = RegOpenKeyExW(rootKey, L"Environment", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) return std::nullopt;

    DWORD type = 0, size = 0;
    result = RegQueryValueExW(hKey, key.c_str(), nullptr, &type, nullptr, &size);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return std::nullopt;
    }

    std::vector<BYTE> buffer(size);
    result = RegQueryValueExW(hKey, key.c_str(), nullptr, &type, buffer.data(), &size);
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) return std::nullopt;

    return std::wstring(reinterpret_cast<WCHAR*>(buffer.data()));
}

std::optional<std::wstring> SysEnvUtil::setKeyValueAndGetOld(const std::wstring& key, const std::wstring& value) const {
    auto oldValue = getKeyValue(key);
    if (!setKeyValue(key, value)) return std::nullopt;
    return oldValue;
}

std::optional<std::wstring> SysEnvUtil::appendToValue(const std::wstring& key, const std::wstring& value) const {
    auto oldValue = getKeyValue(key);
    std::wstring newValue = (oldValue.has_value() ? oldValue.value() + value : value);
    if (!setKeyValue(key, newValue)) return std::nullopt;
    return oldValue;
}