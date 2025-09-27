#ifndef CBACKUP_H
#define CBACKUP_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include "CConfig.h"
#include "CBackupRecorder.h"

class CBackup {
public: 
    CBackup();
    CBackup(const std::string& recorderFilePath);
    ~CBackup();

    // 基本接口实现 --> 就是指定源路径和目标路径的文件迁移
    bool doBackup(const std::string& sourcePath, const std::string& destPath);
    bool doRecovery(const std::string& sourcePath, const std::string& destPath);

    // 拓展接口
    bool doBackup(const std::shared_ptr<CConfig>& config);
    bool doRecovery(const std::shared_ptr<CConfig>& config);

    // 外部获取记录
    const std::vector<BackupEntry>& getBackupRecords() const;

private:
    CBackupRecorder recorder;
    std::string recorderFilePath;
};

// 辅助函数
bool ReadFile(const std::string& filePath, std::vector<char>& buffer);
bool WriteFile(const std::string& filePath, std::vector<char>& buffer);

#endif //CBACKUP_H