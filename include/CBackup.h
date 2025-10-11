#ifndef CBACKUP_H
#define CBACKUP_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <set>  
#include "CConfig.h"
#include "CBackupRecorder.h"

class CBackup {
public: 
    CBackup();
    ~CBackup();

    // 拓展接口
    // 备份相关
    bool doBackup(const std::shared_ptr<CConfig>& config);

    // 恢复相关
    bool doRecovery(const BackupEntry& entry);

private:
    std::set<std::string> createdDirs;  // 用于记录已创建的目录，避免重复创建

};

// 辅助函数
bool ReadFile(const std::string& filePath, std::vector<char>& buffer);
bool WriteFile(const std::string& filePath, std::vector<char>& buffer);
// 避免与 Windows API 宏 CopyFile 冲突
bool CopyFileBinary(const std::string& srcPath, const std::string& destPath);

#endif //CBACKUP_H