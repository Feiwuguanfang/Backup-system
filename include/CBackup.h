#ifndef CBACKUP_H
#define CBACKUP_H

#include <string>
#include <vector>
#include <memory>
#include "CConfig.h"

class CBackup {
public: 
    // 基本接口实现 --> 就是指定源路径和目标路径的文件迁移
    bool doBackup(const std::string& sourcePath, const std::string& destPath);
    bool doRecovery(const std::string& sourcePath, const std::string& destPath);

    // 拓展接口
    bool doBackup(const std::shared_ptr<CConfig>& config);
    bool doRecovery(const std::shared_ptr<CConfig>& config);
};

#endif //CBACKUP_H