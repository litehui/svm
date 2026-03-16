//
// Created by litehui on 26-3-16.
// 删除 SDK 或 SDK 版本功能实现
//

#include "delete.h"
#include "../util/ConfigUtil.h"
#include "../util/SysEnvUtil.h"
#include "backupSysEvn.h"
#include <filesystem>
#include <iostream>
#include <windows.h>

// 使用 FileUtil 类
#include "../util/FileUtil.h"

const std::filesystem::path MODULE_ROOT = FileUtil::getModulePath();

// 原常量改为动态路径
const std::filesystem::path DEFAULT_REPOSITORY_BASE_PATH = MODULE_ROOT / "repository";
const std::filesystem::path DEFAULT_LINK_BASE_PATH = MODULE_ROOT / "link";

// 检查环境变量是否使用了指定的 SDK
bool isSdkInUse(const std::string& sdk, const std::string& version) {
    SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
    SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
    
    std::string sdkUpper = sdk;
    for (char& c : sdkUpper) {
        c = std::toupper(c);
    }
    
    std::string homeEnv = sdkUpper + "_HOME";
    std::wstring wHomeEnv = std::wstring(homeEnv.begin(), homeEnv.end());
    
    std::wstring sysValue = sysEnvUtil.getValue(wHomeEnv);
    std::wstring userValue = userEnvUtil.getValue(wHomeEnv);
    
    if (!sysValue.empty() || !userValue.empty()) {
        return true;
    }
    return false;
}

// 删除环境变量
bool removeEnvironmentVariables(const std::string& sdk) {
    backupSysEvn();
    
    SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
    SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
    
    std::string sdkUpper = sdk;
    for (char& c : sdkUpper) {
        c = std::toupper(c);
    }
    
    std::string homeEnv = sdkUpper + "_HOME";
    std::wstring wHomeEnv = std::wstring(homeEnv.begin(), homeEnv.end());
    
    bool success = true;
    
    // 删除系统环境变量
    if (sysEnvUtil.exists(wHomeEnv)) {
        success &= sysEnvUtil.remove(wHomeEnv);
    }
    
    // 删除用户环境变量
    if (userEnvUtil.exists(wHomeEnv)) {
        success &= userEnvUtil.remove(wHomeEnv);
    }
    
    // 特殊处理 Maven
    if (sdkUpper == "MAVEN") {
        std::wstring wM2Home = L"M2_HOME";
        if (sysEnvUtil.exists(wM2Home)) {
            success &= sysEnvUtil.remove(wM2Home);
        }
        if (userEnvUtil.exists(wM2Home)) {
            success &= userEnvUtil.remove(wM2Home);
        }
    }
    
    return success;
}

// 询问用户是否继续删除
bool confirmDeletion(const std::string& sdk, const std::string& version) {
    std::cout << "Warning: The SDK \"" << sdk << "";
    if (!version.empty()) {
        std::cout << " version " << version;
    }
    std::cout << " is currently in use by system environment variables." << std::endl;
    std::cout << "Do you want to continue deleting? (y/n): ";
    
    char response;
    std::cin >> response;
    return std::tolower(response) == 'y';
}

int removeSdk(const char* sdk, const char* version) {
    try {
        ConfigUtil configUtil("../config/application.properties");

        // 构建路径
        std::filesystem::path basePath = 
            (!std::string{configUtil.getProperty("repositoryBasePath")}.empty() ?
                std::filesystem::path(configUtil.getProperty("repositoryBasePath")) :
                DEFAULT_REPOSITORY_BASE_PATH);
        
        std::filesystem::path targetPath;
        if (version && *version) {
            targetPath = basePath / sdk / version;
        } else {
            targetPath = basePath / sdk;
        }

        // 检查路径是否存在
        if (!std::filesystem::exists(targetPath)) {
            std::wcerr << L"Target path does not exist: " << targetPath.wstring() << std::endl;
            return -1;
        }

        // 检查是否在使用
        if (isSdkInUse(sdk, version ? version : "")) {
            if (!confirmDeletion(sdk, version ? version : "")) {
                std::wcout << L"Deletion cancelled." << std::endl;
                return 0;
            }
            
            // 删除环境变量
            if (!removeEnvironmentVariables(sdk)) {
                std::wcerr << L"Failed to remove environment variables." << std::endl;
                return -1;
            }
        }

        // 删除符号链接
        std::filesystem::path linkPath = 
            (!std::string{configUtil.getProperty("linkBasePath")}.empty() ?
                std::filesystem::path(configUtil.getProperty("linkBasePath")) :
                DEFAULT_LINK_BASE_PATH) / sdk;
        
        if (std::filesystem::exists(linkPath)) {
            std::filesystem::remove(linkPath);
            std::wcout << L"Removed symbolic link: " << linkPath.wstring() << std::endl;
        }

        // 删除目录
        std::filesystem::remove_all(targetPath);
        std::wcout << L"Removed: " << targetPath.wstring() << std::endl;

        // 如果删除的是版本，检查 SDK 目录是否为空
        if (version && *version) {
            std::filesystem::path sdkPath = targetPath.parent_path();
            if (std::filesystem::is_empty(sdkPath)) {
                std::filesystem::remove(sdkPath);
                std::wcout << L"SDK directory is empty, removed: " << sdkPath.wstring() << std::endl;
            }
        }

        std::wcout << L"Successfully deleted SDK: " << sdk;
        if (version && *version) {
            std::wcout << L" version: " << version;
        }
        std::wcout << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::wcerr << L"Config load failed: " << e.what() << std::endl;
        return -1;
    }
}
