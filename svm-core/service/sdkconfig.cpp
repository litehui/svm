//
// Created by litehui on 26-3-16.
// SDK 环境变量配置管理功能实现
//

#include "sdkconfig.h"
#include "../util/StringUtil.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

// 使用 FileUtil 类
#include "../util/FileUtil.h"

const std::filesystem::path MODULE_ROOT = FileUtil::getModulePath();
const std::filesystem::path SDK_CONFIG_FILE = MODULE_ROOT / "config" / "sdk_config.json";

// 保存 SDK 配置到文件
bool saveSdkConfigs(const std::map<std::string, SdkConfig>& configs) {
    try {
        std::ofstream file(SDK_CONFIG_FILE.string());
        if (!file.is_open()) {
            std::wcerr << L"Failed to open SDK config file: " << SDK_CONFIG_FILE.wstring() << std::endl;
            return false;
        }

        file << "{" << std::endl;
        bool first = true;
        for (const auto& [sdk, config] : configs) {
            if (!first) {
                file << "," << std::endl;
            }
            first = false;
            
            file << "  \"" << sdk << "\": {" << std::endl;
            file << "    \"homeEnv\": \"" << config.homeEnv << "\"," << std::endl;
            file << "    \"pathFolders\": [";
            
            bool firstFolder = true;
            for (const auto& folder : config.pathFolders) {
                if (!firstFolder) {
                    file << ", ";
                }
                firstFolder = false;
                file << "\"" << folder << "\"";
            }
            file << "]" << std::endl;
            file << "  }";
        }
        file << std::endl << "}" << std::endl;

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::wcerr << L"Failed to save SDK configs: " << e.what() << std::endl;
        return false;
    }
}

// 从文件加载 SDK 配置
std::map<std::string, SdkConfig> loadSdkConfigs() {
    std::map<std::string, SdkConfig> configs;
    
    // 如果文件不存在，返回默认配置
    if (!std::filesystem::exists(SDK_CONFIG_FILE)) {
        // 设置默认配置
        SdkConfig jdkConfig = {"JAVA_HOME", {"bin"}};
        SdkConfig mavenConfig = {"MAVEN_HOME", {"bin"}};
        SdkConfig gradleConfig = {"GRADLE_HOME", {"bin"}};
        SdkConfig tomcatConfig = {"CATALINA_HOME", {"bin"}};
        
        configs["jdk"] = jdkConfig;
        configs["maven"] = mavenConfig;
        configs["gradle"] = gradleConfig;
        configs["tomcat"] = tomcatConfig;
        
        // 保存默认配置到文件
        saveSdkConfigs(configs);
        return configs;
    }
    
    try {
        std::ifstream file(SDK_CONFIG_FILE.string());
        if (!file.is_open()) {
            std::wcerr << L"Failed to open SDK config file: " << SDK_CONFIG_FILE.wstring() << std::endl;
            return configs;
        }
        
        // 简单的 JSON 解析（仅支持基本格式）
        std::string line;
        std::string content;
        while (std::getline(file, line)) {
            content += line;
        }
        file.close();
        
        // 这里简化处理，实际项目中应该使用 JSON 库
        // 暂时返回默认配置
        SdkConfig jdkConfig = {"JAVA_HOME", {"bin"}};
        SdkConfig mavenConfig = {"MAVEN_HOME", {"bin"}};
        SdkConfig gradleConfig = {"GRADLE_HOME", {"bin"}};
        SdkConfig tomcatConfig = {"CATALINA_HOME", {"bin"}};
        
        configs["jdk"] = jdkConfig;
        configs["maven"] = mavenConfig;
        configs["gradle"] = gradleConfig;
        configs["tomcat"] = tomcatConfig;
        
    } catch (const std::exception& e) {
        std::wcerr << L"Failed to load SDK configs: " << e.what() << std::endl;
    }
    
    return configs;
}

// 分割字符串
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int sdkconfig(const char* sdk, const char* homeEnv, const char* pathFolders) {
    try {
        // 加载现有配置
        auto configs = loadSdkConfigs();
        
        // 创建或更新配置
        SdkConfig config;
        config.homeEnv = homeEnv;
        if (pathFolders) {
            config.pathFolders = split(pathFolders, ',');
        }
        
        configs[sdk] = config;
        
        // 保存配置
        if (!saveSdkConfigs(configs)) {
            return -1;
        }
        
        std::wcout << L"Successfully updated SDK config: " << sdk << std::endl;
        std::wcout << L"  Home environment variable: " << homeEnv << std::endl;
        std::wcout << L"  Path folders: ";
        for (size_t i = 0; i < config.pathFolders.size(); ++i) {
            if (i > 0) std::wcout << L", ";
            std::wcout << StringUtil::utf8_to_utf16(config.pathFolders[i]);
        }
        std::wcout << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::wcerr << L"SDK config update failed: " << e.what() << std::endl;
        return -1;
    }
}

int sdkconfig_show(const char* sdk) {
    try {
        auto configs = loadSdkConfigs();
        
        if (sdk) {
            // 显示指定 SDK 的配置
            auto it = configs.find(sdk);
            if (it != configs.end()) {
                const auto& config = it->second;
                std::wcout << L"SDK: " << sdk << std::endl;
                std::wcout << L"  Home environment variable: " << StringUtil::utf8_to_utf16(config.homeEnv) << std::endl;
                std::wcout << L"  Path folders: ";
                for (size_t i = 0; i < config.pathFolders.size(); ++i) {
                    if (i > 0) std::wcout << L", ";
                    std::wcout << StringUtil::utf8_to_utf16(config.pathFolders[i]);
                }
                std::wcout << std::endl;
            } else {
                std::wcerr << L"SDK not found: " << sdk << std::endl;
                return -1;
            }
        } else {
            // 显示所有 SDK 的配置
            std::wcout << L"=== SDK Environment Variable Configurations ===" << std::endl;
            for (const auto& [sdkName, config] : configs) {
                std::wcout << L"SDK: " << StringUtil::utf8_to_utf16(sdkName) << std::endl;
                std::wcout << L"  Home environment variable: " << StringUtil::utf8_to_utf16(config.homeEnv) << std::endl;
                std::wcout << L"  Path folders: ";
                for (size_t i = 0; i < config.pathFolders.size(); ++i) {
                    if (i > 0) std::wcout << L", ";
                    std::wcout << StringUtil::utf8_to_utf16(config.pathFolders[i]);
                }
                std::wcout << std::endl;
                std::wcout << std::endl;
            }
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::wcerr << L"SDK config show failed: " << e.what() << std::endl;
        return -1;
    }
}

std::map<std::string, SdkConfig> getSdkConfigs() {
    return loadSdkConfigs();
}
