//
// Created by litehui on 25-5-22.
//

#ifndef CONFIGUTIL_H
#define CONFIGUTIL_H

#include <string>
#include <unordered_map>

class ConfigUtil {
public:
    ConfigUtil(const std::string& filePath);
    std::string getProperty(const std::string& key) const;

private:
    void loadProperties(const std::string& filePath);
    std::unordered_map<std::string, std::string> properties;
};

#endif // CONFIGUTIL_H