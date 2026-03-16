//
// Created by litehui on 25-6-18.
//

#include "backupSysEvn.h"
#include "../util/SysEnvUtil.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>


int backupSysEvn(){

    // 生成带时间戳的文件名
    time_t now = time(0);
    struct tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", ltm);
    
    std::string fileName = "../config/backupSysEnv_" + std::string(timestamp) + ".txt";
    std::wofstream outputFile;
    outputFile.open(fileName);

    if(outputFile.is_open()) {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        outputFile << L"last backup date is "
               << std::setfill(L'0') << (1900 + ltm->tm_year) << L"-"
               << std::setw(2) << (1 + ltm->tm_mon) << L"-"
               << std::setw(2) << ltm->tm_mday
               << std::endl;
        outputFile << "this is system enverment====================" << std::endl;
    }

    SysEnvUtil sysEnvUtil(SysEnvUtil::EnvScope::System);
    std::map<std::wstring, std::wstring> allSysEnv = sysEnvUtil.getAll();
    for (auto& entry : allSysEnv) {
        if(outputFile.is_open()) {
            outputFile << entry.first << ": " << entry.second << std::endl;
        }
    }

    if(outputFile.is_open()) {
        outputFile << "this is user enverment====================" << std::endl;
    }

    SysEnvUtil userEnvUtil(SysEnvUtil::EnvScope::User);
    std::map<std::wstring, std::wstring> allUserEnv = userEnvUtil.getAll();
    for (auto& entry : allUserEnv) {
        if(outputFile.is_open()) {
            outputFile << entry.first << ": " << entry.second << std::endl;
        }
    }

    std::cout << "backup your system enverment and user enverment to " << fileName;

    return 0;
}