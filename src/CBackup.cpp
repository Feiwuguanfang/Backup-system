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

// 析构函数
CBackup::~CBackup(){
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


bool CopyFile(const std::string& srcPath, const std::string& destPath){
    // 以二进制进行文件读写
    std::ifstream inFile(srcPath, std::ios::binary); 
    // 检查文件是否打开成功
    if(!inFile.is_open()){
        std::cerr << "Failed to open file: " << srcPath << std::endl;
        return false;
    }
    // 获取文件大小
    inFile.seekg(0, std::ifstream::end);   // 移动到文件末尾
    std::streampos fileSize = inFile.tellg();   // 获取文件大小（当前指针的位置）
    inFile.seekg(0);               // 移动回文件开头
    // 读取文件内容
    std::vector<char> buffer(fileSize);
    if(!inFile.read(buffer.data(), fileSize)){  //  后续可以设置分块读取以防止文件过大导致内存不足
        std::cerr << "Failed to read file: " << srcPath << std::endl;
        return false;
    }
    inFile.close();
    // 写入文件内容
    if(!WriteFile(destPath, buffer)){
        std::cerr << "Failed to write file: " << destPath << std::endl;
        return false;
    }
    return true;
}



bool CBackup::doRecovery(const BackupEntry& entry) {
    std::vector<char> buffer;
    if(!ReadFile(entry.destDirectory + "/" + entry.backupFileName, buffer)){
        std::cerr << "Failed to read file: " << entry.destDirectory + "/" + entry.backupFileName << std::endl;
        return false;
    }

    // 检查是否需要解密
    if(entry.isEncrypted){
        std::cout << "Decrypting file: " << entry.backupFileName << std::endl;
    }

    // 检查是否需要解压
    if(entry.isCompressed){
        std::cout << "Decompressing file: " << entry.backupFileName << std::endl;
    }

    // 检查是否需要解包
    if(entry.isPacked){
        std::cout << "Unpacking file: " << entry.backupFileName << std::endl;
    }

    // 创建目标目录（如果不存在）
    std::filesystem::path destFilePath(entry.sourceFullPath);
    std::filesystem::create_directories(destFilePath.parent_path());
    
    if(!WriteFile(entry.sourceFullPath, buffer)){
        std::cerr << "Failed to write file: " << entry.sourceFullPath << std::endl;
        return false;
    }
    return true;
}


bool CBackup::doBackup(const std::shared_ptr<CConfig>& config) {
    // 检查备份配置是否有效
    if (!config || !config->isValid()) {
        std::cerr << "Error: Invalid backup configuration" << std::endl;
        return false;
    }
    

    std::vector<char> buffer;
    std::vector<std::string> filesToBackup;
    std::string backupName = "backup_" + std::to_string(time(nullptr)); // 生成目标备份文件名

    TODO: 首先筛选文件 放到filesToBackup里面


    // 检查是否有文件
    if(filesToBackup.empty()){
        std::cerr << "Error: No files to backup" << std::endl;
        return false;
    }

    // 创建目标文件夹
    fs::create_directories(config->getDestinationPath());

    // 执行备份操作
    bool success = true;
    std::string backupPath = config->getDestinationPath() + "/" + backupName;


    // 打包操作
    // 通过工厂类创建打包器
    std::unique_ptr<IPack> packer = nullptr;
    try{
        packer = PackFactory::createPacker(config->getPackType());
    }
    catch(const std::exception& e){
        std::cerr << "Error: Failed to create packer: " << e.what() << std::endl;
        return false;
    }

    if(config->isPackingEnabled()){
        // 打包文件
        if(!packer->pack(filesToBackup, backupPath)){ 
            std::cerr << "Error: Failed to pack files" << std::endl;
            success = false;
        }
    }

    else{
        // 那就是直接进行文件copy  -->  这里假设文件类型都是普通文件或者目录  暂时不考虑压缩加密之类的操作

        // 这里主要是要判断源文件是目录还是文件，目录的话需要保持源目录机构  -->  文件列表是通过先根遍历得到的
        // 也就是以源目录为根目录，将相对路径添加到备份中
        // 如果是普通文件
        if(fs::is_regular_file(config->getSourcePath())){
            try{
                std::string destFilePath = backupPath + "/" + fs::path(config->getSourcePath()).filename().string();
                // 确保目标目录存在
            fs::create_directories(fs::path(destFilePath).parent_path());
            fs::copy_file(config->getSourcePath(), destFilePath, fs::copy_options::overwrite_existing);
            } catch (const std::exception& e) {
            std::cerr << "Error copying file " << config->getSourcePath() << " to backup directory: " << e.what() << std::endl;
            success = false;
            }
        }
        else if(fs::is_directory(config->getSourcePath())){
            for(const auto&file : filesToBackup){
                // 构建相对路径和目标路径
                std::string relativePath = fs::relative(file, config->getSourcePath()).string();
                std::string destFilePath = backupPath + "/" + relativePath;

                // 判断当前文件是目录还是文件
                if(fs::is_directory(file)){
                    fs::create_directories(destFilePath);
                    createdDirs.insert(file);
                }
                else{
                    try{
                        // 因为是先根遍历，所以肯定可以保证夫目录存在
                        CopyFile(file, destFilePath);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Error copying file " << file << " to backup directory: " << e.what() << std::endl;
                        success = false;
                    }
                }
            }
        }
    }

    return success;
}
