#include "CBackupRecorder.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem; 

CBackupRecorder::CBackupRecorder()
{
    // 设定默认备份记录文件路径
    CBackupRecorder::recorderFilePath = "backup_records.json";
    // 先检查有没有这个文件
    std::ifstream checkFile(CBackupRecorder::recorderFilePath);
    if(!checkFile.is_open()){
        // 如果文件不存在，创建一个空文件
        std::ofstream createFile(CBackupRecorder::recorderFilePath);
        createFile.close();
    }
    loadBackupRecordsFromFile(CBackupRecorder::recorderFilePath);
    autoSaveEnabled = false;
}

CBackupRecorder::CBackupRecorder(bool autoSave)
{
    // 设定默认备份记录文件路径
    CBackupRecorder::recorderFilePath = "backup_records.json";
    // 先检查有没有这个文件
    std::ifstream checkFile(CBackupRecorder::recorderFilePath);
    if(!checkFile.is_open()){
        // 如果文件不存在，创建一个空文件
        std::ofstream createFile(CBackupRecorder::recorderFilePath);
        createFile.close();
    }
    loadBackupRecordsFromFile(CBackupRecorder::recorderFilePath);
    autoSaveEnabled = autoSave;
}

// 构造函数
CBackupRecorder::CBackupRecorder(const std::string& filePath)
{
    // 检查这个路径是文件还是目录
    // 如果是文件，直接赋值
    CBackupRecorder::recorderFilePath = filePath;
    // 是目录，则将默认文件名加在目录后面
    // 调用的时候应该还没创建，直接看路径最后有没有json后缀
    if(filePath.find(".json") == std::string::npos){
        CBackupRecorder::recorderFilePath = filePath + "/" + "backup_records.json";
    }
    loadBackupRecordsFromFile(recorderFilePath);
}

CBackupRecorder::~CBackupRecorder()
{
    if(autoSaveEnabled){
        saveBackupRecordsToFile(CBackupRecorder::recorderFilePath);
    }
}


// 这个读取是会直接进行覆盖的
bool CBackupRecorder::loadBackupRecordsFromFile(const std::string& filePath){
    try{
        // 读取文件
        std::ifstream file(filePath);
        if(!file.is_open()){
            std::cerr << "Error: Failed to open file " << filePath << " for reading." << std::endl;
            return false;
        }
        nlohmann::json j;
        file >> j;

        // 将原先的记录清空
        backupRecords.clear();

        // 将json数据格式转换
        if(j.is_array()){
            backupRecords = j.get<std::vector<BackupEntry>>();
        }

        file.close();
        return true;
    }catch(const std::exception& e){
        std::cerr << "Error: Failed to load backup records from file " << filePath << ". Exception: " << e.what() << std::endl;
        return false;
    }
}


bool CBackupRecorder::saveBackupRecordsToFile(const std::string& filePath){
    try{
        // 写入文件
        std::ofstream file(filePath);
        if(!file.is_open()){
            std::cerr << "Error: Failed to open file " << filePath << " for writing." << std::endl;
            return false;
        }
        nlohmann::json j = backupRecords;
        file << j.dump(4); // 4 表示缩进空格数
        file.close();
        return true;
    }catch(const std::exception& e){
        std::cerr << "Error: Failed to save backup records to file " << filePath << ". Exception: " << e.what() << std::endl;
        return false;
    }
}

// 直接将条目添加进来
void CBackupRecorder::addBackupRecord(const BackupEntry& entry){
    backupRecords.push_back(entry);
}

const std::vector<BackupEntry>& CBackupRecorder::getBackupRecords() const{
    return backupRecords;
}

std::vector<BackupEntry> CBackupRecorder::findBackupRecordsByFileName(const std::string& queryFileName) const{
    std::vector<BackupEntry> result;
    for(const auto& entry : backupRecords){
        // 支持模糊搜索：检查文件名是否包含查询字符串
        if(entry.fileName.find(queryFileName) != std::string::npos){
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<BackupEntry> CBackupRecorder::findBackupRecordsByBackupTime(const std::string& startime, const std::string& endTime) const{
    std::vector<BackupEntry> result;
    for(const auto& entry : backupRecords){
        if(entry.backupTime >= startime && entry.backupTime <= endTime){
            result.push_back(entry);
        }
    }
    return result;
}

// 检查索引是否有效
bool CBackupRecorder::isIndexValid(size_t index) const{
    return index < backupRecords.size();
}

// 根据备份记录获取全局索引
size_t CBackupRecorder::getBackupRecordIndex(const BackupEntry& entry) const{
    auto it = std::find(backupRecords.begin(), backupRecords.end(), entry);
    if(it != backupRecords.end()){
        return std::distance(backupRecords.begin(), it);
    }
    return std::string::npos;
}

// 删除备份文件或目录的辅助函数
static bool deleteBackupFile(const BackupEntry& entry) {
    try {
        // 直接读取记录的目标路径递归删除
        fs::path backupDirPath = fs::path(entry.destDirectory);
        fs::path backupFilePath = backupDirPath / entry.backupFileName;
        
        bool deleted = false;
        
        // 首先尝试删除整个 destDirectory 目录（对于目录备份，这是整个备份的根目录）
        if (fs::exists(backupDirPath) && fs::is_directory(backupDirPath)) {
            // 递归删除整个目录
            fs::remove_all(backupDirPath);
            std::cout << "Deleted backup directory (recursive): " << backupDirPath.string() << std::endl;
            deleted = true;
        }
        // 如果 destDirectory 不存在或不是目录，尝试删除 backupFilePath（打包的文件）
        else if (fs::exists(backupFilePath)) {
            if (fs::is_directory(backupFilePath)) {
                fs::remove_all(backupFilePath);
                std::cout << "Deleted backup directory: " << backupFilePath.string() << std::endl;
                deleted = true;
            } else if (fs::is_regular_file(backupFilePath)) {
                fs::remove(backupFilePath);
                std::cout << "Deleted backup file: " << backupFilePath.string() << std::endl;
                deleted = true;
            } else {
                fs::remove(backupFilePath);
                std::cout << "Deleted backup item: " << backupFilePath.string() << std::endl;
                deleted = true;
            }
        }
        
        if (!deleted) {
            std::cerr << "Warning: Backup file/directory not found: " << backupDirPath.string() 
                      << " or " << backupFilePath.string() << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting backup file: " << e.what() << std::endl;
        return false;
    }
}

bool CBackupRecorder::deleteBackupRecord(size_t index){
    if(!isIndexValid(index)){
        std::cerr << "Error: Invalid index " << index << " for deleting backup record." << std::endl;
        return false;
    }
    
    // 在删除记录前，先删除对应的备份文件
    const BackupEntry& entry = backupRecords[index];
    deleteBackupFile(entry);
    
    // 删除记录
    backupRecords.erase(backupRecords.begin() + index);
    return true;
}

bool CBackupRecorder::deleteBackupRecord(const BackupEntry& entry){
    size_t index = getBackupRecordIndex(entry);
    if(index == std::string::npos){
        std::cerr << "Error: Backup record not found for deletion." << std::endl;
        return false;
    }
    
    // 在删除记录前，先删除对应的备份文件（使用实际记录）
    const BackupEntry& actualEntry = backupRecords[index];
    deleteBackupFile(actualEntry);
    
    // 删除记录
    backupRecords.erase(backupRecords.begin() + index);
    return true;
}

bool CBackupRecorder::modifyBackupRecord(size_t index, const BackupEntry& newEntry){
    if(isIndexValid(index)){
        backupRecords[index] = newEntry;
        return true;
    }
    std::cerr << "Error: Invalid index " << index << " for modifying backup record." << std::endl;
    return false;
}

bool CBackupRecorder::modifyBackupRecord(const BackupEntry& oldEntry, const BackupEntry& newEntry){
    size_t index = getBackupRecordIndex(oldEntry);
    if(index != std::string::npos){
        return modifyBackupRecord(index, newEntry);
    }
    std::cerr << "Error: Backup record not found for modification." << std::endl;
    return false;
}

std::string CBackupRecorder::getRecorderFilePath() const{
    return recorderFilePath;
}

void CBackupRecorder::addBackupRecord(const std::shared_ptr<CConfig>& config, const std::string& destPath){
    BackupEntry entry;
    // 之前已经将配置中的路径转换为了绝对路径
    std::string sourcePath = config->getSourcePath();
    // 文件名就是绝对路径的文件名
    std::string fileName = fs::path(sourcePath).filename().string();
    // 完整的目标路径
    std::string destDir = config->getDestinationPath();
    // 最后备份的文件名通过destPath得到
    std::string backupFileName = fs::path(destPath).filename().string();
    // 记录时间，精确到分钟
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    char timeBuffer[64];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", tm);
    std::string backupTime = timeBuffer;
    // 通过配置文件查看是否有加密等设置
    bool isEncrypted = config->isEncryptionEnabled();
    bool isPacked = config->isPackingEnabled();
    bool isCompressed = config->isCompressionEnabled();
    entry = BackupEntry(fileName, sourcePath, destDir, backupFileName, backupTime, isEncrypted, isPacked, isCompressed);
    // 增加备份记录
    backupRecords.push_back(entry);
}


    // std::string fileName;        // 源文件名
    // std::string sourceFullPath;  // 源文件完整路径
    // std::string destDirectory;   // 备份目标目录
    // std::string backupFileName;  // 最终备份文件名
    // std::string backupTime;      // 备份时间
    // bool isEncrypted;            // 是否加密
    // bool isPacked;               // 是否打包
    // bool isCompressed;           // 是否压缩