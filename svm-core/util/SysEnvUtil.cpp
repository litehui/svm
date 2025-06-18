#include "SysEnvUtil.h"
#include <algorithm>

SysEnvUtil::SysEnvUtil(EnvScope scope) : m_scope(scope) {
    m_rootKey = (scope == EnvScope::System) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    m_regPath = (scope == EnvScope::System) 
        ? L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" 
        : L"Environment";
}

SysEnvUtil::~SysEnvUtil() {}

bool SysEnvUtil::openKey(HKEY* hKey, REGSAM permissions) const {
    return RegOpenKeyExW(m_rootKey, m_regPath.c_str(), 0, permissions, hKey) == ERROR_SUCCESS;
}

void SysEnvUtil::broadcastChange() const {
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 
                       (LPARAM)L"Environment", SMTO_BLOCK, 100, nullptr);
}

std::map<std::wstring, std::wstring> SysEnvUtil::getAll() const {
    std::map<std::wstring, std::wstring> result;
    HKEY hKey;
    if (!openKey(&hKey, KEY_READ)) return result;

    DWORD index = 0;
    wchar_t name[32767];
    wchar_t data[32767];
    DWORD nameSize, dataSize, type;

    while (true) {
        nameSize = 32767;
        dataSize = 32767 * sizeof(wchar_t);
        if (RegEnumValueW(hKey, index, name, &nameSize, nullptr, &type, 
                         (LPBYTE)data, &dataSize) != ERROR_SUCCESS) {
            break;
        }
        
        if (type == REG_SZ || type == REG_EXPAND_SZ) {
            result[name] = data;
        }
        index++;
    }

    RegCloseKey(hKey);
    return result;
}

bool SysEnvUtil::remove(const std::wstring& key) {
    HKEY hKey;
    if (!openKey(&hKey, KEY_WRITE)) return false;

    bool success = (RegDeleteValueW(hKey, key.c_str()) == ERROR_SUCCESS);
    RegCloseKey(hKey);

    if (success) broadcastChange();
    return success;
}

bool SysEnvUtil::exists(const std::wstring& key) const {
    HKEY hKey;
    if (!openKey(&hKey, KEY_READ)) return false;

    DWORD type, size = 0;
    bool exists = (RegQueryValueExW(hKey, key.c_str(), nullptr, &type, nullptr, &size) == ERROR_SUCCESS);
    RegCloseKey(hKey);
    return exists;
}

bool SysEnvUtil::renameKey(const std::wstring& oldKey, const std::wstring& newKey) {
    std::wstring oldValue = getValue(oldKey);
    if (oldValue.empty()) return false;
    if (!remove(oldKey)) return false;
    return setValue(newKey, oldValue);
}

bool SysEnvUtil::setValue(const std::wstring& key, const std::wstring& value) {
    HKEY hKey;
    if (!openKey(&hKey, KEY_WRITE)) return false;

    bool success = (RegSetValueExW(hKey, key.c_str(), 0, REG_SZ, 
                                 (const BYTE*)value.c_str(), 
                                 (value.size() + 1) * sizeof(wchar_t)) == ERROR_SUCCESS);
    RegCloseKey(hKey);
    
    if (success) broadcastChange();
    return success;
}

std::wstring SysEnvUtil::getValue(const std::wstring& key) const {
    HKEY hKey;
    if (!openKey(&hKey, KEY_READ)) return L"";

    wchar_t buffer[32767];
    DWORD size = sizeof(buffer);
    DWORD type;
    
    if (RegQueryValueExW(hKey, key.c_str(), nullptr, &type, 
                        (LPBYTE)buffer, &size) != ERROR_SUCCESS || 
        (type != REG_SZ && type != REG_EXPAND_SZ)) {
        RegCloseKey(hKey);
        return L"";
    }

    RegCloseKey(hKey);
    return std::wstring(buffer);
}

std::wstring SysEnvUtil::setValueAndGetOld(const std::wstring& key, const std::wstring& value) {
    std::wstring oldValue = getValue(key);
    setValue(key, value);
    return oldValue;
}

std::wstring SysEnvUtil::appendValue(const std::wstring& key, const std::wstring& value, const wchar_t separator) {
    std::wstring oldValue = getValue(key);
    std::wstring newValue = oldValue;
    
    if (!oldValue.empty() && oldValue.back() != separator) {
        newValue += separator;
    }
    newValue += value;
    
    setValue(key, newValue);
    return oldValue;
}