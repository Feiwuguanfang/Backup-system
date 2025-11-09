#ifndef CBACKUP_H
#define CBACKUP_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <set>  
#include <fstream>
#include <iostream>

#include "CConfig.h"
#include "CBackupRecorder.h"

#include "PackFactory.h"  
#include "CompressFactory.h"
#include "EncryptFactory.h"
namespace fs = std::filesystem; 



class CBackup {
public: 
    CBackup();
    ~CBackup();

    // 拓展接口
    // 备份相关
    std::string doBackup(const std::shared_ptr<CConfig>& config);

    // 恢复相关
    bool doRecovery(const BackupEntry& entry, const std::string& destDir);
    // 恢复相关（带密码参数，用于GUI）
    bool doRecovery(const BackupEntry& entry, const std::string& destDir, const std::string& password);


private:
    std::set<std::string> createdDirs;  // 用于记录已创建的目录，避免重复创建

};

// 辅助函数
bool ReadFile(const std::string& filePath, std::vector<char>& buffer);
bool WriteFile(const std::string& filePath, std::vector<char>& buffer);
// 避免与 Windows API 宏 CopyFile 冲突
bool CopyFileBinary(const std::string& srcPath, const std::string& destPath);
// 新增：收集需要备份的文件列表
std::vector<std::string> collectFilesToBackup(const std::string& rootPath, const std::shared_ptr<CConfig>& config);


#endif //CBACKUP_H