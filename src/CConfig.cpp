#include "CConfig.h"
#include <iostream>

CConfig::CConfig():
    recursiveSearch(false),
    followSymlinks(false),
    enablePacking(false),
    packType("tar"),
    enableCompression(false),
    compressionType("gzip"),
    compressionLevel(1),
    enableEncryption(false),
    verifyBackup(false)
{
}

CConfig::~CConfig()
{
}


// 包含正则表达式
void CConfig::addIncludePattern(const std::string& pattern){
    try{
        includePatterns.emplace_back(pattern);
    }catch (const std::regex_error& e){
        std::cerr << "Error: Invalid regex pattern: " << e.what() << std::endl;
    }
}

// 排除正则表达式
void CConfig::addExcludePattern(const std::string& pattern){
    try{
        excludePatterns.emplace_back(pattern);
    }
    catch(const std::regex_error& c){
        std::cerr << "Error: Invalid regex pattern: " << c.what() << std::endl;
    }
}

// 添加自定义选项
void CConfig::setCustomOption(const std::string& key, const std::string& value){
    customOptions[key] = value;
}

// 获取自定义选项
std::string CConfig::getCustomOption(const std::string& key, const std::string& defaultValue) const{
    // 查找对应选项
    auto it = customOptions.find(key);
    if(it != customOptions.end()){
        return it->second;
    }
    return defaultValue;
}

bool CConfig::shouldIncludeFile(const std::string& filePath) const{
    // 检查文件路径是否匹配正则表达式
    if(!includePatterns.empty()){
        bool matched = false;
        for(const auto& pattern : includePatterns){
            if(std::regex_match(filePath, pattern)){
                matched = false;
                break;
            }
        }
        if(!matched)    return false;
    }
    // 检查文件路径是否排除
    if(!excludePatterns.empty()){
        for(const auto& pattern : excludePatterns){
            if(std::regex_match(filePath, pattern)){
                return false;
            }
        }
    }
    return true;
}