#ifndef CBACKUPMANAGER_H
#define CBACKUPMANAGER_H

#include <memory>
#include <string>   // 补充 std::string 头文件
#include <vector>   // 补充 std::vector 头文件

// 前向声明
class CBackup;
class CConfig;
struct BackupEntry;

#include "CBackupRecorder.h"

class CBackupManager {
public:
    CBackupManager();
    CBackupManager(const std::string& recorderFilePath);
    ~CBackupManager();

    // 备份管理接口
    bool doBackup(const std::shared_ptr<CConfig>& inputConfig);  // 重命名参数避免冲突
    bool doRecovery(const BackupEntry& entry);

    // 获取备份记录（保持不变）
    const std::vector<BackupEntry>& getBackupRecords() const { return recorder->getBackupRecords(); };
    std::vector<BackupEntry> findBackupRecordsByFileName(const std::string& queryFileName) const { return recorder->findBackupRecordsByFileName(queryFileName); };
    std::vector<BackupEntry> findBackupRecordsByBackupTime(const std::string& startime, const std::string& endTime) const { return recorder->findBackupRecordsByBackupTime(startime, endTime); };
    bool deleteBackupRecord(const BackupEntry& entry) { return recorder->deleteBackupRecord(entry); };
    bool loadBackupRecordsFromFile(const std::string& filePath) { return recorder->loadBackupRecordsFromFile(filePath); };
    bool saveBackupRecordsToFile(const std::string& filePath) { return recorder->saveBackupRecordsToFile(filePath); };

    // 获取底层组件（保持不变）
    std::shared_ptr<CBackup> getBackupComponent() const { return backup; };
    std::shared_ptr<CBackupRecorder> getRecorderComponent() const { return recorder; };
    std::shared_ptr<CConfig> getConfigComponent() const { return config; };

private:
    std::shared_ptr<CBackup> backup;
    std::shared_ptr<CBackupRecorder> recorder;
    std::shared_ptr<CConfig> config;
    static std::string GetCurrentTime_();  // 与实现保持一致
};
#endif
