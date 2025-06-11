//
// Created by litehui on 25-5-22.
//

#include "ConfigUtil.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

ConfigUtil::ConfigUtil(const std::string& filePath) {
    loadProperties(filePath);
}

std::string ConfigUtil::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return "";
}

void ConfigUtil::loadProperties(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            properties[key] = value;
        }
    }

    file.close();
}