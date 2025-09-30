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
    ~CBackup();

    // 拓展接口
    // 备份只和设置相关
    bool doBackup(const std::shared_ptr<CConfig>& config);

    // 恢复只和记录相关
    bool doRecovery(const BackupEntry& entry);

private:


};

// 辅助函数
bool ReadFile(const std::string& filePath, std::vector<char>& buffer);
bool WriteFile(const std::string& filePath, std::vector<char>& buffer);
bool CopyFile(const std::string& srcPath, const std::string& destPath);

#endif //CBACKUP_H