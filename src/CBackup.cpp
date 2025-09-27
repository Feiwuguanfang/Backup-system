#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "CBackup.h"

/**
 * CBackup implementation
 */

// build 去搭建项目框架  Cmake  --> 项目一些开发环境

// 构造函数
CBackup::CBackup(){
}

CBackup::CBackup(const std::string& recorderFilePath){
    recorder.loadBackupRecordsFromFile(recorderFilePath);
}

// 析构函数
CBackup::~CBackup(){
    recorder.saveBackupRecordsToFile(recorderFilePath);
}



// 构建一个读写的辅助函数
bool ReadFile(const std::string& filePath, std::vector<char>& buffer){
    // 以二进制进行文件读写
    std::ifstream inFile(filePath, std::ios::binary); 
    // 检查文件是否打开成功
    if(!inFile.is_open()){
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    // 获取文件大小
    inFile.seekg(0, std::ifstream::end);   // 移动到文件末尾
    std::streampos fileSize = inFile.tellg();   // 获取文件大小（当前指针的位置）
    inFile.seekg(0);               // 移动回文件开头
    // 读取文件内容
    buffer.resize(fileSize);
    if(!inFile.read(buffer.data(), fileSize)){  //  后续可以设置分块读取以防止文件过大导致内存不足
        std::cerr << "Failed to read file: " << filePath << std::endl;
        return false;
    }
    return true;
}

std::string GetCurrentTime(){
    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}


bool WriteFile(const std::string& filePath, std::vector<char>& buffer){
    // 以二进制进行写操作
    std::ofstream outFile(filePath, std::ofstream::binary);
    if(!outFile.is_open()){
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    outFile.write(buffer.data(), buffer.size());
    if(!outFile){
        std::cerr << "Failed to write file: " << filePath << std::endl;
        return false;
    }
    outFile.close();
    return true;
}


bool CBackup::doBackup(const std::string& sourcePath, const std::string& destPath){
    std::vector<char> buffer;
    if(!ReadFile(sourcePath, buffer)){
        std::cerr << "Failed to read file: " << sourcePath << std::endl;
        return false;
    }
    if(!WriteFile(destPath, buffer)){
        std::cerr << "Failed to write file: " << destPath << std::endl;
        return false;
    }
    // 记录备份记录
    std::string fileName = sourcePath.substr(sourcePath.find_last_of("/") + 1);
    BackupEntry entry = {fileName, destPath, GetCurrentTime(), "backup"};
    recorder.addBackupRecord(entry);
    return true;
}

// 外部获取记录
const std::vector<BackupEntry>& CBackup::getBackupRecords() const{
    return recorder.getBackupRecords();
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