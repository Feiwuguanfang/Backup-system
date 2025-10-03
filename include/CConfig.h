#ifndef CCONFIG_H
#define CCONFIG_H

#include <string>
#include <vector>
#include <regex>
#include <memory>

// 配置类，负责存储和管理备份系统的所有配置项
class CConfig {
public:
    // 默认构造函数
    CConfig();
    
    // 完整构造函数，直接提供所有必要参数
    CConfig(const std::string& sourcePath, const std::string& destinationPath);
    
    // 析构函数
    ~CConfig();
    
    // 允许拷贝构造和赋值
    CConfig(const CConfig&) = default;
    CConfig& operator=(const CConfig&) = default;
    
    // 允许移动构造和移动赋值
    CConfig(CConfig&&) noexcept = default;
    CConfig& operator=(CConfig&&) noexcept = default;
    
    // ===== 基本配置设置器和获取器 =====
    // 设置源路径列表 (必须)
    CConfig& setSourcePath(const std::string& path);
    const std::string& getSourcePath() const;
    
    // 设置目标路径 (必须)
    CConfig& setDestinationPath(const std::string& path);
    const std::string& getDestinationPath() const;
    
    // ===== 文件筛选配置 =====
    CConfig& setRecursiveSearch(bool value);
    bool isRecursiveSearch() const;
    
    CConfig& setFollowSymlinks(bool value);
    bool isFollowSymlinks() const;
    
    CConfig& addIncludePattern(const std::string& pattern);
    const std::vector<std::regex>& getIncludePatterns() const;
    
    CConfig& addExcludePattern(const std::string& pattern);
    const std::vector<std::regex>& getExcludePatterns() const;
    
    // ===== 备份行为配置 =====
    CConfig& setPackingEnabled(bool value);
    bool isPackingEnabled() const;
    
    CConfig& setPackType(const std::string& type);
    const std::string& getPackType() const;
    
    CConfig& setCompressionEnabled(bool value);
    bool isCompressionEnabled() const;
    
    CConfig& setCompressionType(const std::string& type);
    const std::string& getCompressionType() const;
    
    CConfig& setCompressionLevel(int level);
    int getCompressionLevel() const;
    
    CConfig& setEncryptionEnabled(bool value);
    bool isEncryptionEnabled() const;
    
    CConfig& setEncryptionKey(const std::string& key);
    const std::string& getEncryptionKey() const;
    
    // ===== 高级配置 =====
    CConfig& setCustomOption(const std::string& key, const std::string& value);
    std::string getCustomOption(const std::string& key, const std::string& defaultValue = "") const;
    
    // ===== 便捷方法 =====
    // 检查文件是否应该被包含
    bool shouldIncludeFile(const std::string& filePath) const;
    
    // 验证配置的有效性
    bool isValid() const;
    
    // 重置配置为默认状态
    void reset();
    
    // 深拷贝配置
    std::shared_ptr<CConfig> clone() const;
    
    // 输出配置摘要
    std::string toString() const;
    
private:
    // 基本配置
    std::string sourcePath;
    std::string destinationPath;
    
    // 文件筛选配置
    bool recursiveSearch;
    bool followSymlinks;
    std::vector<std::regex> includePatterns;
    std::vector<std::regex> excludePatterns;
    // 后面还可能需要增加筛选的条件

    // 比如文件大小范围

    // 文件修改时间范围
    
    // 备份行为配置
    bool enablePacking;
    std::string packType;
    bool enableCompression;
    std::string compressionType;
    int compressionLevel;
    bool enableEncryption;
    std::string encryptionKey;
    
    // 高级配置
    std::map<std::string, std::string> customOptions;
};

#endif // CCONFIG_H