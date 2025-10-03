# include "CBackupManager.h"

CBackupManager::CBackupManager()
{
    backup = std::make_shared<CBackup>();
    recorder = std::make_shared<CBackupRecorder>();
    config = std::make_shared<CConfig>();
}

CBackupManager::CBackupManager(const std::string& recorderFilePath)
{
    backup = std::make_shared<CBackup>();
    recorder = std::make_shared<CBackupRecorder>(recorderFilePath);
    config = std::make_shared<CConfig>();
}

CBackupManager::~CBackupManager()
{
}

std::string GetCurrentTime(){
    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}



bool CBackupManager::doBackup(const std::shared_ptr<CConfig>& config){
    if(!config){
        std::cerr << "Error: Config is null in doBackup." << std::endl;
        return false;
    }
    // 执行备份
    bool success = backup->doBackup(config);
    if(!success){
        std::cerr << "Error: Backup failed." << std::endl;
        return false;
    }
    // 记录备份
    // 备份可能是批量进行备份
    for(const auto& sourcePath : config->getSourcePaths()){
        std::string fileName = sourcePath.substr(sourcePath.find_last_of("/") + 1);
        std::string destPath = config->getDestinationPath() + "/" + fileName; // 简单拼接，后续可以改进
        // 备份记录中是否需要记录加密、打包、压缩信息
        bool isEncrypted = config->isEnableEncryption();
        bool isPacked = config->isEnablePacking();
        bool isCompressed = config->isEnableCompression();
        BackupEntry entry = {fileName, destPath, GetCurrentTime(), isEncrypted, isPacked, isCompressed};
        recorder->addBackupRecord(entry);
    }
    return true;
}


