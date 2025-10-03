#ifndef CBACKUPMANAGER_H
#define CBACKUPMANAGER_H

#include <memory>
#include "CBakup.h"
#include "CBackupRecorder.h"
#include "CConfig.h"

class CBackupManager {
public:
    CBackupManager();
    CBackupManager(const std::string& recorderFilePath);
    ~CBackupManager();

    // 备份管理接口
    bool doBackup(const std::shared_ptr<CConfig>& config);
    bool doRecovery(const BackupEntry& entry);


    // 获取备份记录
    const std::vector<BackupEntry>& getBackupRecords() const { return recorder->getBackupRecords(); };
    // 根据文件名查找备份记录
    std::vector<BackupEntry> findBackupRecordsByFileName(const std::string& queryFileName) const { return recorder->findBackupRecordsByFileName(queryFileName); };
    // 根据备份时间查找条目
    std::vector<BackupEntry> findBackupRecordsByBackupTime(const std::string& startime, const std::string& endTime) const; { return recorder->findBackupRecordsByBackupTime(startime, endTime); };
    // 根据备份记录删除条目
    bool deleteBackupRecord(const BackupEntry& entry) { return recorder->deleteBackupRecord(entry); };
    // 加载备份记录
    bool loadBackupRecordsFromFile(const std::string& filePath) { return recorder->loadBackupRecordsFromFile(filePath); };
    // 保存备份记录
    bool saveBackupRecordsToFile(const std::string& filePath) { return recorder->saveBackupRecordsToFile(filePath); };

    // 获取底层组件
    std::shared_ptr<CBackup> getBackupComponent() const { return backup; };
    std::shared_ptr<CBackupRecorder> getRecorderComponent() const { return recorder; };
    std::shared_ptr<CConfig> getConfigComponent() const { return config; };

private:
    // 底层组件
    std::shared_ptr<CBackup> backup;
    std::shared_ptr<CBackupRecorder> recorder;
    std::shared_ptr<CConfig> config;
};
#endif