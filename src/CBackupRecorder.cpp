#include "CBackupRecorder.h"
#include <fstream>
#include <algorithm>
#include <iostream>


CBackupRecorder::CBackupRecorder()
{
    // 设定默认备份记录文件路径
    recorderFilePath = "backup_records.json";
    // 先检查有没有这个文件
    std::ifstream checkFile(recorderFilePath);
    if(!checkFile.is_open()){
        // 如果文件不存在，创建一个空文件
        std::ofstream createFile(recorderFilePath);
        createFile.close();
    }
    loadBackupRecordsFromFile(recorderFilePath);
}

// 构造函数
CBackupRecorder::CBackupRecorder(const std::string& filePath)
{
    recorderFilePath = filePath;
    loadBackupRecordsFromFile(recorderFilePath);
}

CBackupRecorder::~CBackupRecorder()
{
    saveBackupRecordsToFile(recorderFilePath);
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
        json j;
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
        json j = backupRecords;
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
        if(entry.fileName == queryFileName){
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

bool CBackupRecorder::deleteBackupRecord(size_t index){
    if(isIndexValid(index)){
        backupRecords.erase(backupRecords.begin() + index);
        return true;
    }
    std::cerr << "Error: Invalid index " << index << " for deleting backup record." << std::endl;
    return false;
}

bool CBackupRecorder::deleteBackupRecord(const BackupEntry& entry){
    size_t index = getBackupRecordIndex(entry);
    if(index != std::string::npos){
        backupRecords.erase(backupRecords.begin() + index);
        return true;
    }
    std::cerr << "Error: Backup record not found for deletion." << std::endl;
    return false;
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