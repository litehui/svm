//
// Created by litehui on 26-3-16.
// 配置管理功能实现
//

#include "config.h"
#include "../util/ConfigUtil.h"
#include "../util/SysEnvUtil.h"
#include "../util/StringUtil.h"
#include "backupSysEvn.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <windows.h>

// 使用 FileUtil 类
#include "../util/FileUtil.h"

const std::filesystem::path MODULE_ROOT = FileUtil::getModulePath();
const std::filesystem::path CONFIG_FILE = MODULE_ROOT / "config" / "application.properties";

// 原常量改为动态路径
const std::filesystem::path DEFAULT_REPOSITORY_BASE_PATH = MODULE_ROOT / "repository";
const std::filesystem::path DEFAULT_LINK_BASE_PATH = MODULE_ROOT / "link";

// 使用 FileUtil::copyDirectory

// 读取配置文件
std::pair<std::string, std::string> readConfig() {
    ConfigUtil configUtil(CONFIG_FILE.string());
    std::string repoPath = configUtil.getProperty("repositoryBasePath");
    std::string linkPath = configUtil.getProperty("linkBasePath");
    return {repoPath, linkPath};
}

// 写入配置文件
bool writeConfig(const std::string& repoPath, const std::string& linkPath) {
    try {
        std::ofstream file(CONFIG_FILE.string());
        if (!file.is_open()) {
            std::wcerr << L"Failed to open config file: " << CONFIG_FILE.wstring() << std::endl;
            return false;
        }

        file << "# when you change linkBasePath, you must restart your computer, after run \"use\" function." << std::endl;
        file << "# path example:C:\\svm\\repository" << std::endl;
        file << "repositoryBasePath=" << repoPath << std::endl;
        file << "linkBasePath=" << linkPath << std::endl;

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::wcerr << L"Failed to write config file: " << e.what() << std::endl;
        return false;
    }
}

// 处理符号链接更新
bool updateSymbolicLinks(const std::filesystem::path& oldLinkPath, const std::filesystem::path& newLinkPath, const std::filesystem::path& repoPath) {
    if (!std::filesystem::exists(oldLinkPath)) {
        return true; // 旧链接路径不存在，不需要更新
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(oldLinkPath)) {
            if (entry.is_symlink()) {
                std::filesystem::path linkName = entry.path().filename();
                std::filesystem::path target = std::filesystem::read_symlink(entry.path());
                
                // 构建新的链接路径
                std::filesystem::path newLink = newLinkPath / linkName;
                std::filesystem::create_directories(newLink.parent_path());
                
                // 删除旧链接（如果存在）
                if (std::filesystem::exists(newLink)) {
                    std::filesystem::remove(newLink);
                }
                
                // 创建新链接
                std::wstring command = L"mklink /d \"" + newLink.wstring() + L"\" \"" + target.wstring() + L"\"";
                int result = _wsystem(command.c_str());
                if (result != 0) {
                    std::wcerr << L"Failed to create symlink: " << newLink.wstring() << std::endl;
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::wcerr << L"Failed to update symbolic links: " << e.what() << std::endl;
        return false;
    }
}

// 处理环境变量更新
void updateEnvironmentVariables(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) {
    backupSysEvn();
    
    SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
    SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
    
    // 检查并更新常见的环境变量
    const std::vector<std::string> envVars = {"JAVA_HOME", "MAVEN_HOME", "M2_HOME", "GRADLE_HOME", "CATALINA_HOME"};
    
    for (const auto& envVar : envVars) {
        std::wstring wEnvVar = std::wstring(envVar.begin(), envVar.end());
        
        // 更新系统环境变量
        std::wstring sysValue = sysEnvUtil.getValue(wEnvVar);
        if (!sysValue.empty()) {
            std::filesystem::path sysPath(sysValue);
            if (sysPath.string().find(oldPath.string()) != std::string::npos) {
                std::string sysValueStr = StringUtil::utf16_to_utf8(sysValue);
                std::string newPathStr = sysValueStr;
                size_t pos = newPathStr.find(oldPath.string());
                if (pos != std::string::npos) {
                    newPathStr.replace(pos, oldPath.string().length(), newPath.string());
                    sysEnvUtil.setValue(wEnvVar, StringUtil::utf8_to_utf16(newPathStr));
                    std::wcout << L"Updated system environment variable " << wEnvVar << L" from " << sysValue << L" to " << StringUtil::utf8_to_utf16(newPathStr) << std::endl;
                }
            }
        }
        
        // 更新用户环境变量
        std::wstring userValue = userEnvUtil.getValue(wEnvVar);
        if (!userValue.empty()) {
            std::filesystem::path userPath(userValue);
            if (userPath.string().find(oldPath.string()) != std::string::npos) {
                std::string userValueStr = StringUtil::utf16_to_utf8(userValue);
                std::string newPathStr = userValueStr;
                size_t pos = newPathStr.find(oldPath.string());
                if (pos != std::string::npos) {
                    newPathStr.replace(pos, oldPath.string().length(), newPath.string());
                    userEnvUtil.setValue(wEnvVar, StringUtil::utf8_to_utf16(newPathStr));
                    std::wcout << L"Updated user environment variable " << wEnvVar << L" from " << userValue << L" to " << StringUtil::utf8_to_utf16(newPathStr) << std::endl;
                }
            }
        }
    }
}

int config(const char* key, const char* value) {
    try {
        // 读取当前配置
        auto [currentRepoPath, currentLinkPath] = readConfig();
        
        // 确定当前路径
        std::filesystem::path oldRepoPath = currentRepoPath.empty() ? DEFAULT_REPOSITORY_BASE_PATH : std::filesystem::path(currentRepoPath);
        std::filesystem::path oldLinkPath = currentLinkPath.empty() ? DEFAULT_LINK_BASE_PATH : std::filesystem::path(currentLinkPath);
        
        std::string newRepoPath = currentRepoPath;
        std::string newLinkPath = currentLinkPath;
        
        // 处理 repositoryBasePath 配置
        if (strcmp(key, "repositoryBasePath") == 0) {
            newRepoPath = value;
            std::filesystem::path newPath(value);
            
            // 复制 repository 目录
            if (!FileUtil::copyDirectory(oldRepoPath, newPath)) {
                return -1;
            }
            
            std::wcout << L"Copied repository from " << oldRepoPath.wstring() << L" to " << newPath.wstring() << std::endl;
            
            // 更新环境变量中的路径
            updateEnvironmentVariables(oldRepoPath, newPath);
        }
        
        // 处理 linkBasePath 配置
        else if (strcmp(key, "linkBasePath") == 0) {
            newLinkPath = value;
            std::filesystem::path newPath(value);
            
            // 复制 link 目录
            if (!FileUtil::copyDirectory(oldLinkPath, newPath)) {
                return -1;
            }
            
            std::wcout << L"Copied link from " << oldLinkPath.wstring() << L" to " << newPath.wstring() << std::endl;
            
            // 更新符号链接
            if (!updateSymbolicLinks(oldLinkPath, newPath, oldRepoPath)) {
                return -1;
            }
            
            // 更新环境变量中的路径
            updateEnvironmentVariables(oldLinkPath, newPath);
        }
        
        // 写入新配置
        if (!writeConfig(newRepoPath, newLinkPath)) {
            return -1;
        }
        
        std::wcout << L"Successfully updated config: " << key << L" = " << value << std::endl;
        std::wcout << L"Note: If you changed linkBasePath, you may need to restart your computer for changes to take effect." << std::endl;
        
        return 0;

    } catch (const std::exception& e) {
        std::wcerr << L"Config update failed: " << e.what() << std::endl;
        return -1;
    }
}
