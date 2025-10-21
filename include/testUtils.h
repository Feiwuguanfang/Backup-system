#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// 辅助函数：创建测试文件
inline bool CreateTestFile(const std::string& filePath, const std::string& content) {
    try {
        // 确保目录存在
        fs::path dir = fs::path(filePath).parent_path();
        if (!dir.empty() && !fs::exists(dir)) {
            fs::create_directories(dir);
        }
        
        // 创建并写入文件
        std::ofstream file(filePath, std::ios::binary);
        if (!file) return false;
        file.write(content.data(), content.size());
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

// 辅助函数：读取测试文件内容
inline bool ReadTestFile(const std::string& filePath, std::vector<char>& buffer) {
    try {
        if (!fs::exists(filePath)) return false;
        
        // 获取文件大小
        std::uintmax_t size = fs::file_size(filePath);
        buffer.resize(static_cast<size_t>(size));
        
        // 读取文件内容
        std::ifstream file(filePath, std::ios::binary);
        if (!file) return false;
        file.read(buffer.data(), buffer.size());
        return file.gcount() == static_cast<std::streamsize>(buffer.size());
    } catch (...) {
        return false;
    }
}

// 辅助函数：清理测试文件
inline void CleanupTestFile(const std::string& filePath) {
    try {
        if (fs::exists(filePath)) {
            fs::remove(filePath);
        }
    } catch (...) {
        // 忽略清理过程中的错误
    }
}

#endif // TEST_UTILS_H