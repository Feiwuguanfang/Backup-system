#include "MyCompress.h"
#include <stdexcept>  // 用于异常处理

// 构造函数：初始化默认压缩级别
MyCompress::MyCompress() : m_compressionLevel(6) {}  // 默认级别6

// 压缩文件实现（示例：返回true表示成功，实际需补充压缩逻辑）
bool MyCompress::compressFile(const std::string& sourcePath, const std::string& destPath) {
    // 实际压缩逻辑（如调用zlib、7z等库）
    (void)sourcePath;  // 避免未使用参数警告
    (void)destPath;    // 避免未使用参数警告
    return true;
}

// 解压缩文件实现
bool MyCompress::decompressFile(const std::string& sourcePath, const std::string& destPath) {
    // 实际解压缩逻辑
    (void)sourcePath;  // 避免未使用参数警告
    (void)destPath;    // 避免未使用参数警告
    return true;
}

// 压缩内存数据实现
bool MyCompress::compressData(const std::vector<char>& sourceData, std::vector<char>& destData) {
    // 实际内存压缩逻辑
    (void)sourceData;  // 避免未使用参数警告
    (void)destData;    // 避免未使用参数警告
    return true;
}

// 解压缩内存数据实现
bool MyCompress::decompressData(const std::vector<char>& sourceData, std::vector<char>& destData) {
    // 实际内存解压缩逻辑
    (void)sourceData;  // 避免未使用参数警告
    (void)destData;    // 避免未使用参数警告
    return true;
}

// 设置压缩级别（检查范围1-9）
void MyCompress::setCompressionLevel(int level) {
    if (level < 1 || level > 9) {
        throw std::invalid_argument("Compression level must be between 1 and 9");
    }
    m_compressionLevel = level;
}

// 返回当前压缩算法的文件扩展名（如"myz"）
std::string MyCompress::getFileExtension() const {
    return "myz";  // 自定义扩展名
}