#include <vector>
#include <string>
#include "CBackup.h"

/**
 * CBackup implementation
 */

// build 去搭建项目框架  Cmake  --> 项目一些开发环境


bool CBackup::doBackup(const std::string& sourcePath, const std::string& destPath){
    return true;
}

bool CBackup::doRecovery(const std::string& sourcePath, const std::string& destPath){
    return true;
}


bool CBackup::doBackup(const std::shared_ptr<CConfig>& config) {
    return true;
}

bool CBackup::doRecovery(const std::shared_ptr<CConfig>& config) {
    return true;
}