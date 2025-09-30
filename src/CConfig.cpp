#include "CConfig.h"
#include <sstream>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <map>

// 默认构造函数
CConfig::CConfig() {
    // 设置默认值
    reset();
}

// 完整构造函数
CConfig::CConfig(const std::string& sourcePath, const std::string& destinationPath) {
    reset();
    setSourcePath(sourcePath);
    setDestinationPath(destinationPath);
}

// 析构函数
CConfig::~CConfig() {
    // 清理资源
}

// 设置源路径列表
CConfig& CConfig::setSourcePath(const std::string& path) {
    if (path.empty()) {
        throw std::invalid_argument("Source path cannot be empty");
    }
    sourcePath = path;
    return *this;  // 链式设计,避免重复引用
}

// 获取源路径列表
const std::string& CConfig::getSourcePath() const {
    return sourcePath;
}


// 设置目标路径
CConfig& CConfig::setDestinationPath(const std::string& path) {
    if (path.empty()) {
        throw std::invalid_argument("Destination path cannot be empty");
    }
    destinationPath = path;
    return *this;
}

// 获取目标路径
const std::string& CConfig::getDestinationPath() const {
    return destinationPath;
}

// 设置递归搜索
CConfig& CConfig::setRecursiveSearch(bool value) {
    recursiveSearch = value;
    return *this;
}

// 获取递归搜索设置
bool CConfig::isRecursiveSearch() const {
    return recursiveSearch;
}

// 设置是否遵循符号链接
CConfig& CConfig::setFollowSymlinks(bool value) {
    followSymlinks = value;
    return *this;
}

// 获取是否遵循符号链接设置
bool CConfig::isFollowSymlinks() const {
    return followSymlinks;
}

// 添加包含模式
CConfig& CConfig::addIncludePattern(const std::string& pattern) {
    try {
        includePatterns.emplace_back(pattern);
    } catch (const std::regex_error& e) {
        std::cerr << "Warning: Invalid regex pattern: " << e.what() << std::endl;
        // 继续执行，忽略无效的正则表达式
    }
    return *this;
}

// 获取包含模式列表
const std::vector<std::regex>& CConfig::getIncludePatterns() const {
    return includePatterns;
}

// 添加排除模式
CConfig& CConfig::addExcludePattern(const std::string& pattern) {
    try {
        excludePatterns.emplace_back(pattern);
    } catch (const std::regex_error& e) {
        std::cerr << "Warning: Invalid regex pattern: " << e.what() << std::endl;
        // 继续执行，忽略无效的正则表达式
    }
    return *this;
}

// 获取排除模式列表
const std::vector<std::regex>& CConfig::getExcludePatterns() const {
    return excludePatterns;
}

// 设置启用打包
CConfig& CConfig::setPackingEnabled(bool value) {
    enablePacking = value;
    return *this;
}

// 获取是否启用打包
bool CConfig::isPackingEnabled() const {
    return enablePacking;
}

// 设置打包类型
CConfig& CConfig::setPackType(const std::string& type) {
    packType = type;
    return *this;
}

// 获取打包类型
const std::string& CConfig::getPackType() const {
    return packType;
}

// 设置启用压缩
CConfig& CConfig::setCompressionEnabled(bool value) {
    enableCompression = value;
    return *this;
}

// 获取是否启用压缩
bool CConfig::isCompressionEnabled() const {
    return enableCompression;
}

// 设置压缩类型
CConfig& CConfig::setCompressionType(const std::string& type) {
    compressionType = type;
    return *this;
}

// 获取压缩类型
const std::string& CConfig::getCompressionType() const {
    return compressionType;
}

// 设置压缩级别
CConfig& CConfig::setCompressionLevel(int level) {
    if (level < 1 || level > 9) {
        throw std::invalid_argument("Compression level must be between 1 and 9");
    }
    compressionLevel = level;
    return *this;
}

// 获取压缩级别
int CConfig::getCompressionLevel() const {
    return compressionLevel;
}

// 设置启用加密
CConfig& CConfig::setEncryptionEnabled(bool value) {
    enableEncryption = value;
    return *this;
}

// 获取是否启用加密
bool CConfig::isEncryptionEnabled() const {
    return enableEncryption;
}

// 设置加密密钥
CConfig& CConfig::setEncryptionKey(const std::string& key) {
    encryptionKey = key;
    return *this;
}

// 获取加密密钥
const std::string& CConfig::getEncryptionKey() const {
    return encryptionKey;
}

// 设置自定义选项
CConfig& CConfig::setCustomOption(const std::string& key, const std::string& value) {
    customOptions[key] = value;
    return *this;
}

// 获取自定义选项
std::string CConfig::getCustomOption(const std::string& key, const std::string& defaultValue) const {
    auto it = customOptions.find(key);
    if (it != customOptions.end()) {
        return it->second;
    }
    return defaultValue;
}

// 检查文件是否应该被包含
bool CConfig::shouldIncludeFile(const std::string& filePath) const {
    // 检查排除模式
    if (!excludePatterns.empty()) {
        for (const auto& pattern : excludePatterns) {
            if (std::regex_match(filePath, pattern)) {
                return false;
            }
        }
    }
    
    // 检查包含模式（如果指定了）
    if (!includePatterns.empty()) {
        for (const auto& pattern : includePatterns) {
            if (std::regex_match(filePath, pattern)) {
                return true;
            }
        }
        return false; // 如果有包含模式但没有匹配，则排除
    }
    
    return true; // 默认包含
}

// 验证配置的有效性
bool CConfig::isValid() const {
    // 检查必要参数
    if (sourcePath.empty()) {
        std::cerr << "Error: Source path cannot be empty" << std::endl;
        return false;
    }
    
    if (destinationPath.empty()) {
        std::cerr << "Error: Destination path cannot be empty" << std::endl;
        return false;
    }
    
    // 检查源路径是否存在
    if (!std::filesystem::exists(sourcePath)) {
        std::cerr << "Error: Source path does not exist: " << sourcePath << std::endl;
        return false;
    }
    
    // 检查目标路径是否存在，如果不存在则尝试创建
    if (!std::filesystem::exists(destinationPath)) {
        try {
            if (!std::filesystem::create_directories(destinationPath)) {
                std::cerr << "Error: Failed to create destination directory: " << destinationPath << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to create destination directory: " << e.what() << std::endl;
            return false;
        }
    }
    
    // 检查压缩级别是否在有效范围内
    if (compressionLevel < 1 || compressionLevel > 9) {
        std::cerr << "Error: Compression level must be between 1 and 9" << std::endl;
        return false;
    }
    
    return true;
}

// 重置配置为默认状态
void CConfig::reset() {
    sourcePath.clear();
    destinationPath.clear();
    recursiveSearch = false;
    followSymlinks = false;
    includePatterns.clear();
    excludePatterns.clear();
    enablePacking = false;
    packType = "tar";
    enableCompression = false;
    compressionType = "gzip";
    compressionLevel = 1;
    enableEncryption = false;
    encryptionKey.clear();
    customOptions.clear();
}

// 深拷贝配置
std::shared_ptr<CConfig> CConfig::clone() const {
    return std::make_shared<CConfig>(*this);
}

// 输出配置摘要
std::string CConfig::toString() const {
    std::ostringstream oss;
    oss << "Backup Configuration:" << std::endl;
    oss << "  Source Path: " << sourcePath << std::endl;
    oss << "  Destination Path: " << destinationPath << std::endl;
    oss << "  Recursive Search: " << (recursiveSearch ? "Yes" : "No") << std::endl;
    oss << "  Follow Symlinks: " << (followSymlinks ? "Yes" : "No") << std::endl;
    oss << "  Enable Packing: " << (enablePacking ? "Yes (" + packType + ")" : "No") << std::endl;
    oss << "  Enable Compression: " << (enableCompression ? "Yes (" + compressionType + ", Level " + std::to_string(compressionLevel) + ")" : "No") << std::endl;
    oss << "  Enable Encryption: " << (enableEncryption ? "Yes" : "No") << std::endl;
    
    if (!includePatterns.empty()) {
        oss << "  Include Patterns: " << includePatterns.size() << std::endl;
    }
    if (!excludePatterns.empty()) {
        oss << "  Exclude Patterns: " << excludePatterns.size() << std::endl;
    }
    if (!customOptions.empty()) {
        oss << "  Custom Options: " << customOptions.size() << std::endl;
    }
    
    return oss.str();
}