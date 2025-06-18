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

    std::wofstream outputFile;
    outputFile.open("../config/backupSysEnv.txt");

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

    std::cout << "backup your system enverment and user enverment to config/backupSysEnv.txt";

    return 0;
}