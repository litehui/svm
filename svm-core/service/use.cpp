#include "use.h"
#include "../util/ConfigUtil.h"
#include "backupSysEvn.h"
#include "../util/SysEnvUtil.h"
#include "sdkconfig.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <windows.h> // 新增头文件


// 使用 FileUtil 类
#include "../util/FileUtil.h"

const std::filesystem::path MODULE_ROOT = FileUtil::getModulePath();


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

bool updateEnvironment(std::wstring target, SysEnvUtil& envUtil) {
    std::wstring pathValue = envUtil.getValue(L"Path");
    std::wstring upperTarget = target;
    std::wstring upperPathValue = pathValue;

    auto toUpper = [](wchar_t c) { return std::towupper(c); };
    std::transform(upperTarget.begin(), upperTarget.end(), upperTarget.begin(), toUpper);
    std::transform(upperPathValue.begin(), upperPathValue.end(), upperPathValue.begin(), toUpper);

    if (upperPathValue.find(upperTarget) == std::wstring::npos) {
        std::wstring result = envUtil.appendValue(L"Path", target);
    }
    return true;
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

        // 加载 SDK 配置
        auto sdkConfigs = getSdkConfigs();
        auto it = sdkConfigs.find(sdk);
        
        if (it != sdkConfigs.end()) {
            const auto& config = it->second;
            
            backupSysEvn();
            
            // 更新系统环境变量
            SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
            std::wstring wHomeEnv = std::wstring(config.homeEnv.begin(), config.homeEnv.end());
            if (!sysEnvUtil.setValue(wHomeEnv, linkPath.wstring())) {
                return -1;
            }
            
            // 更新系统 Path
            for (const auto& folder : config.pathFolders) {
                std::wstring pathEntry = L"%" + wHomeEnv + L"%\\" + std::wstring(folder.begin(), folder.end());
                if (!updateEnvironment(pathEntry, sysEnvUtil)) {
                    return -1;
                }
            }
            
            // 更新用户环境变量
            SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
            if (!userEnvUtil.setValue(wHomeEnv, linkPath.wstring())) {
                return -1;
            }
            
            // 更新用户 Path
            for (const auto& folder : config.pathFolders) {
                std::wstring pathEntry = L"%" + wHomeEnv + L"%\\" + std::wstring(folder.begin(), folder.end());
                if (!updateEnvironment(pathEntry, userEnvUtil)) {
                    return -1;
                }
            }
            
            // 特殊处理 Maven
            if (strcmp(sdk, "maven") == 0) {
                // 设置 M2_HOME
                std::wstring wM2Home = L"M2_HOME";
                if (!sysEnvUtil.setValue(wM2Home, linkPath.wstring())) {
                    return -1;
                }
                if (!userEnvUtil.setValue(wM2Home, linkPath.wstring())) {
                    return -1;
                }
            }
        } else {
            std::wcerr << L"SDK config not found for: " << sdk << std::endl;
            return -1;
        }


        // else if (strcmp(sdk,"tomcat")==0) {
        //     backupSysEvn();
        //     SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
        //     if (!sysEnvUtil.setValue(L"CATALINA_HOME", linkPath.wstring()) ||
        //     !updateEnvironment(L"%CATALINA_HOME%\\bin",sysEnvUtil)) {
        //         return -1;
        //     }
        //     SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
        //     if (!userEnvUtil.setValue(L"CATALINA_HOME", linkPath.wstring()) ||
        //     !updateEnvironment(L"%CATALINA_HOME%\\bin",userEnvUtil)) {
        //         return -1;
        //     }
        // }



        return 0;

    } catch (const std::exception& e) {
        std::wcerr << L"Config load failed: " << e.what() << std::endl;
        return -1;
    }
}