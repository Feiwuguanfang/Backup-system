#ifndef CCONFIG_H
#define CCONFIG_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <iostream>

// 配置类，用于存储备份相关的配置，后续需要在这里进行增添
// 配置类，用于存储备份相关的配置
class CConfig {
public:
    CConfig();
    ~CConfig();
    
    // ===== 基本配置 =====
    // 源路径列表
    std::vector<std::string> sourcePaths;
    
    // 目标路径
    std::string destinationPath;
    
    // ===== 文件筛选配置 =====
    // 包含的文件正则表达式列表
    std::vector<std::regex> includePatterns;
    
    // 排除的文件正则表达式列表
    std::vector<std::regex> excludePatterns;
    
    // 是否递归搜索子目录
    bool recursiveSearch;
    
    // 是否遵循符号链接
    bool followSymlinks;
    
    // ===== 备份行为配置 =====
    // 是否启用打包
    bool enablePacking;
    
    // 打包类型
    std::string packType;
    
    // 是否启用压缩
    bool enableCompression;
    
    // 压缩类型
    std::string compressionType;
    
    // 压缩级别 (1-9)
    int compressionLevel;
    
    // 是否启用加密
    bool enableEncryption;
    
    // 加密密钥
    std::string encryptionKey;
    
    // ===== 验证和日志配置 =====
    // 是否验证备份完整性
    bool verifyBackup;
    
    // 日志级别 (0: 无, 1: 错误, 2: 警告, 3: 信息, 4: 详细)
    int logLevel;
    
    // ===== 高级配置 =====
    // 自定义选项的键值对存储
    std::map<std::string, std::string> customOptions;
    
    // ===== 便捷方法 =====
    // 添加包含模式
    void addIncludePattern(const std::string& pattern);
    
    // 添加排除模式
    void addExcludePattern(const std::string& pattern);
    
    // 设置自定义选项
    void setCustomOption(const std::string& key, const std::string& value);
    
    // 获取自定义选项
    std::string getCustomOption(const std::string& key, const std::string& defaultValue = "") const;
    
    // 检查文件是否应该被包含
    bool shouldIncludeFile(const std::string& filePath) const;
};

#endif // _CCONFIG_H