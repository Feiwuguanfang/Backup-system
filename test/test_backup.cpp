#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "CBackup.h"

// 辅助函数定义必须放在测试用例之前
bool CreateTestFile(const std::string& filePath, const std::string& content) {
    try {
        // 确保目录存在
        std::filesystem::path dir = std::filesystem::path(filePath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
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

void CleanupTestFile(const std::string& filePath) {
    try {
        if (std::filesystem::exists(filePath)) {
            std::filesystem::remove(filePath);
        }
    } catch (...) {
        // 忽略清理错误
    }
}

TEST(BackupTest, BasicBackup){  
    // 测试准备：创建测试文件
    const std::string sourcePath = "test_file.txt";
    const std::string destDir = "test_dir";
    const std::string destPath = destDir + "/test_file.txt";
    const std::string testContent = "This is a test content for backup system.";

    // 确保目标目录存在
    if (!std::filesystem::exists(destDir)) {
        std::filesystem::create_directories(destDir);
    }
    
    // 创建源文件
    ASSERT_TRUE(CreateTestFile(sourcePath, testContent)) << "Failed to create test file";

    CBackup backup;
    bool result = backup.doBackup(sourcePath, destPath);
    EXPECT_TRUE(result) << "Backup failed";

    // 检查备份文件是否存在
    std::ifstream backupFile(destPath);
    EXPECT_TRUE(backupFile.is_open()) << "Backup file not found";
    backupFile.close();
    
    // 检查备份文件内容是否与源文件相同
    std::vector<char> sourceBuffer, backupBuffer;
    bool readSource = ReadFile(sourcePath, sourceBuffer);
    bool readBackup = ReadFile(destPath, backupBuffer);
    EXPECT_TRUE(readSource) << "Failed to read source file";
    EXPECT_TRUE(readBackup) << "Failed to read backup file";
    EXPECT_EQ(sourceBuffer, backupBuffer) << "Backup content mismatch";

    // 测试记录
    const std::vector<BackupEntry>& records = backup.getBackupRecords();
    EXPECT_EQ(records.size(), 1) << "Backup record not found";
    EXPECT_EQ(records[0].fileName, "test_file.txt");
    EXPECT_EQ(records[0].destPath, destPath);

    // 测试清理
    CleanupTestFile(sourcePath);
    CleanupTestFile(destPath);
}