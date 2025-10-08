#include "CBackupManager.h"
#include "CBackup.h"
#include "CConfig.h"
#include <ctime>
#include <iostream>
#include <windows.h>  // 若使用 localtime_s（Windows 平台）
#include <filesystem> // 若使用 std::filesystem（C++17 及以上）

namespace fs = std::filesystem;  // 命名空间别名

CBackupManager::CBackupManager() {
    backup = std::make_shared<CBackup>();
    recorder = std::make_shared<CBackupRecorder>();
    config = std::make_shared<CConfig>();
}

CBackupManager::CBackupManager(const std::string& recorderFilePath) {
    backup = std::make_shared<CBackup>();
    recorder = std::make_shared<CBackupRecorder>(recorderFilePath);
    config = std::make_shared<CConfig>();
}

CBackupManager::~CBackupManager() {}

// 补充 static 关键字，匹配类内声明
    std::string CBackupManager::GetCurrentTime_() {
    std::time_t now = std::time(nullptr);
    char buffer[20];
    struct tm timeinfo;
    // Windows 线程安全版本（替代 std::localtime）
    localtime_s(&timeinfo, &now);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return std::string(buffer);
}

bool CBackupManager::doBackup(const std::shared_ptr<CConfig>& inputConfig) {  // 参数名修正
    if (!inputConfig) {
        std::cerr << "Error: Config is null in doBackup." << std::endl;
        return false;
    }

    // 执行备份
    bool success = backup->doBackup(inputConfig);  // 使用修正后的参数名
    if (!success) {
        std::cerr << "Error: Backup failed." << std::endl;
        return false;
    }

    // 记录备份（使用 std::filesystem 拼接路径）
    for (const auto& sourcePath : inputConfig->getSourcePaths()) {
        fs::path srcPath(sourcePath);
        std::string fileName = srcPath.filename().string();  // 跨平台获取文件名

        fs::path destDir(inputConfig->getDestinationPath());
        fs::path fullDestPath = destDir / fileName;  // 跨平台路径拼接
        std::string destPath = fullDestPath.string();

        // 构建备份文件名（保持逻辑）
        std::string backupFile = fileName;
        if (inputConfig->isPackingEnabled()) {
            backupFile += "." + inputConfig->getPackType();
        }

        // 构建备份记录
        BackupEntry entry = {
            fileName,
            sourcePath,
            destPath,
            backupFile,
            GetCurrentTime_(),
            inputConfig->isEncryptionEnabled(),
            inputConfig->isPackingEnabled(),
            inputConfig->isCompressionEnabled()
        };
        recorder->addBackupRecord(entry);
    }

    return true;
}