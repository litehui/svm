#include "use.h"
#include <algorithm>
#include <filesystem>
#include "../util/ConfigUtil.h"
#include "../util/SysEnvUtil.h"
#include <iostream>
#include <windows.h> // 新增头文件


// 替换原有常量定义
std::filesystem::path getModulePath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().parent_path(); // 获取项目根目录
}

const std::filesystem::path MODULE_ROOT = getModulePath();


// 原常量改为动态路径
const std::filesystem::path DEFAULT_REPOSITORY_BASE_PATH = MODULE_ROOT / "repository";
const std::filesystem::path DEFAULT_LINK_BASE_PATH = MODULE_ROOT / "link";

// 创建符号链接
bool createSymbolicLink(const std::filesystem::path& linkPath, const std::filesystem::path& targetPath) {
    if (std::filesystem::exists(linkPath)) {
        std::filesystem::remove(linkPath);
    }

    std::wstring command = L"mklink /d \"" + linkPath.wstring() + L"\" \"" + targetPath.wstring() + L"\"";
    int result = _wsystem(command.c_str());

    if (result != 0) {
        std::wcerr << L"Failed to create symlink with error code: " << result << std::endl;
        return false;
    }
    return true;
}

// 检查路径是否已存在于PATH环境变量
bool isPathInEnvironment(const std::vector<std::wstring>& paths, const std::wstring& targetPath) {
    return std::any_of(paths.begin(), paths.end(),
        [&targetPath](const std::wstring& path) {
            return path == targetPath;
        });
}

// 解析PATH环境变量
std::vector<std::wstring> parsePathEnvironment(const std::wstring& pathValue) {
    std::vector<std::wstring> paths;
    size_t start = 0;
    size_t end = pathValue.find(L";");

    while (end != std::wstring::npos) {
        paths.push_back(pathValue.substr(start, end - start));
        start = end + 1;
        end = pathValue.find(L";", start);
    }
    paths.push_back(pathValue.substr(start)); // 添加最后一个路径项
    return paths;
}

int use(const char* sdk, const char* version) {

    try {
        ConfigUtil configUtil("..\\config\\application.properties");

        // 构建路径
        std::filesystem::path repositoryPath =
            (!std::string{configUtil.getProperty("repositoryBasePath")}.empty() ?
                std::filesystem::path(configUtil.getProperty("repositoryBasePath")) :
                DEFAULT_REPOSITORY_BASE_PATH) / sdk / version;

        std::filesystem::path linkPath =
            (!std::string{configUtil.getProperty("linkBasePath")}.empty() ?
                std::filesystem::path(configUtil.getProperty("linkBasePath")) :
                DEFAULT_LINK_BASE_PATH) / sdk;

        // 检查路径是否存在
        if (!std::filesystem::exists(repositoryPath)) {
            std::wcerr << L"repositoryPath not exists" << std::endl;
            return -1;
        }


        // 创建符号链接
        if (!createSymbolicLink(linkPath, repositoryPath)) {
            return -1;
        }

        // 设置环境变量
        SysEnvUtil userEnv(EnvType::User);

        // 设置JAVA_HOME
        userEnv.setKeyValue(L"JAVA_HOME", linkPath.wstring());

        // 处理PATH环境变量
        auto pathValue = userEnv.getKeyValue(L"PATH");
        if (!pathValue.has_value()) {
            std::wcerr << L"PATH environment variable not found." << std::endl;
            return -1;
        }

        std::vector<std::wstring> paths =
            parsePathEnvironment(pathValue.value());

        std::wstring targetPath = L"%JAVA_HOME%\\bin";
        if (!isPathInEnvironment(paths, targetPath)) {
            std::wstring targetToAdd = L";%JAVA_HOME%\\bin";
            if (pathValue->empty() || pathValue->back() == L';') {
                targetToAdd = L"%JAVA_HOME%\\bin";
            }

            if (!userEnv.appendToValue(L"PATH", targetToAdd).has_value()) {
                std::wcerr << L"Failed to append target path to PATH." << std::endl;
            }
        }

        return 0;

    } catch (const std::exception& e) {
        std::wcerr << L"Config load failed: " << e.what() << std::endl;
        return -1;
    }
}