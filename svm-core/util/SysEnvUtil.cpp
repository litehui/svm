#include "SysEnvUtil.h"
#include "StringUtil.h"
#include <algorithm>

#ifdef _WIN32
// Windows 平台实现
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
#else
// Linux 平台实现
#include <cstdlib>
#include <fstream>
#include <sstream>

SysEnvUtil::SysEnvUtil(EnvScope scope) : m_scope(scope) {}

SysEnvUtil::~SysEnvUtil() {}

std::map<std::wstring, std::wstring> SysEnvUtil::getAll() const {
    std::map<std::wstring, std::wstring> result;
    
    // 读取当前环境变量
    extern char** environ;
    for (char** env = environ; *env != nullptr; ++env) {
        std::string env_str = *env;
        size_t pos = env_str.find('=');
        if (pos != std::string::npos) {
            std::string key = env_str.substr(0, pos);
            std::string value = env_str.substr(pos + 1);
            result[StringUtil::utf8_to_utf16(key)] = StringUtil::utf8_to_utf16(value);
        }
    }
    
    return result;
}

bool SysEnvUtil::remove(const std::wstring& key) {
    std::string key_utf8 = StringUtil::utf16_to_utf8(key);
    return unsetenv(key_utf8.c_str()) == 0;
}

bool SysEnvUtil::exists(const std::wstring& key) const {
    std::string key_utf8 = StringUtil::utf16_to_utf8(key);
    return getenv(key_utf8.c_str()) != nullptr;
}

bool SysEnvUtil::setValue(const std::wstring& key, const std::wstring& value) {
    std::string key_utf8 = StringUtil::utf16_to_utf8(key);
    std::string value_utf8 = StringUtil::utf16_to_utf8(value);
    return setenv(key_utf8.c_str(), value_utf8.c_str(), 1) == 0;
}

std::wstring SysEnvUtil::getValue(const std::wstring& key) const {
    std::string key_utf8 = StringUtil::utf16_to_utf8(key);
    char* value = getenv(key_utf8.c_str());
    if (value) {
        return StringUtil::utf8_to_utf16(value);
    }
    return L"";
}
#endif

// 平台无关的方法
bool SysEnvUtil::renameKey(const std::wstring& oldKey, const std::wstring& newKey) {
    std::wstring oldValue = getValue(oldKey);
    if (oldValue.empty()) return false;
    if (!remove(oldKey)) return false;
    return setValue(newKey, oldValue);
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

// ========== UTF-8 接口实现 ==========

std::map<std::string, std::string> SysEnvUtil::getAllUtf8() const {
    std::map<std::wstring, std::wstring> wresult = getAll();
    std::map<std::string, std::string> result;
    for (const auto& entry : wresult) {
        result[StringUtil::utf16_to_utf8(entry.first)] = StringUtil::utf16_to_utf8(entry.second);
    }
    return result;
}

bool SysEnvUtil::removeUtf8(const std::string& key) {
    return remove(StringUtil::utf8_to_utf16(key));
}

bool SysEnvUtil::existsUtf8(const std::string& key) const {
    return exists(StringUtil::utf8_to_utf16(key));
}

bool SysEnvUtil::renameKeyUtf8(const std::string& oldKey, const std::string& newKey) {
    return renameKey(StringUtil::utf8_to_utf16(oldKey), StringUtil::utf8_to_utf16(newKey));
}

bool SysEnvUtil::setValueUtf8(const std::string& key, const std::string& value) {
    return setValue(StringUtil::utf8_to_utf16(key), StringUtil::utf8_to_utf16(value));
}

std::string SysEnvUtil::getValueUtf8(const std::string& key) const {
    return StringUtil::utf16_to_utf8(getValue(StringUtil::utf8_to_utf16(key)));
}

std::string SysEnvUtil::setValueAndGetOldUtf8(const std::string& key, const std::string& value) {
    return StringUtil::utf16_to_utf8(setValueAndGetOld(StringUtil::utf8_to_utf16(key), StringUtil::utf8_to_utf16(value)));
}

std::string SysEnvUtil::appendValueUtf8(const std::string& key, const std::string& value, const char separator) {
    return StringUtil::utf16_to_utf8(appendValue(StringUtil::utf8_to_utf16(key), StringUtil::utf8_to_utf16(value), static_cast<wchar_t>(separator)));
}