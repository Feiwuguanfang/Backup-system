﻿#include <gtest/gtest.h>

#include "CBackup.h"
#include "CBackupManager.h"
#include "CConfig.h"

#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include "testUtils.h"


TEST(BackupTest, BasicBackup) {  
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

    // 创建配置对象
    auto config = std::make_shared<CConfig>(sourcePath, destDir);
    
    CBackup backup;
    bool result = backup.doBackup(config);
    EXPECT_TRUE(result) << "Backup failed";

    // 检查备份文件是否存在
    std::ifstream backupFile(destPath);
    EXPECT_TRUE(backupFile.is_open()) << "Backup file not found";
    backupFile.close();
    
    // 检查备份文件内容是否与源文件相同
    std::vector<char> sourceBuffer, backupBuffer;
    bool readSource = ReadTestFile(sourcePath, sourceBuffer);
    bool readBackup = ReadTestFile(destPath, backupBuffer);
    EXPECT_TRUE(readSource) << "Failed to read source file";
    EXPECT_TRUE(readBackup) << "Failed to read backup file";
    EXPECT_EQ(sourceBuffer, backupBuffer) << "Backup content mismatch";

    // 测试记录 - 需要通过CBackupManager来获取记录
    CBackupManager manager;
    const std::vector<BackupEntry>& records = manager.getBackupRecords();
    // 注意：由于我们直接使用CBackup，没有通过CBackupManager，所以这里可能没有记录
    // 这是测试设计的问题，需要重新设计测试逻辑

    // 测试清理
    CleanupTestFile(sourcePath);
    CleanupTestFile(destPath);
}