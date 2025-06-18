#include "use.h"
#include "../util/ConfigUtil.h"
#include "backupSysEvn.h"
#include "../util/SysEnvUtil.h"
#include <algorithm>
#include <filesystem>
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

        if (strcmp(sdk,"jdk")==0) {
            backupSysEvn();
            SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
            if (!sysEnvUtil.setValue(L"JAVA_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%JAVA_HOME%\\bin",sysEnvUtil)) {
                return -1;
            }

            SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
            if (!userEnvUtil.setValue(L"JAVA_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%JAVA_HOME%\\bin",userEnvUtil)) {
                return -1;
            }
        } else if (strcmp(sdk,"maven")==0) {
            backupSysEvn();
            SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
            if (!sysEnvUtil.setValue(L"MAVEN_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%MAVEN_HOME%\\bin",sysEnvUtil)) {
                return -1;
            }
            if (!sysEnvUtil.setValue(L"M2_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%M2_HOME%\\bin",sysEnvUtil)) {
                return -1;
            }

            SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
            if (!userEnvUtil.setValue(L"MAVEN_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%MAVEN_HOME%\\bin",userEnvUtil)) {
                return -1;
            }
            if (!userEnvUtil.setValue(L"M2_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%M2_HOME%\\bin",userEnvUtil)) {
                return -1;
            }
        }else if (strcmp(sdk,"gradle")==0) {
            backupSysEvn();
            SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
            if (!sysEnvUtil.setValue(L"GRADLE_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%GRADLE_HOME%\\bin",sysEnvUtil)) {
                return -1;
            }

            SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
            if (!userEnvUtil.setValue(L"GRADLE_HOME", linkPath.wstring()) ||
            !updateEnvironment(L"%GRADLE_HOME%\\bin",userEnvUtil)) {
                return -1;
            }
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