﻿#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "PackFactory.h"  
#include "CompressFactory.h"
#include "CBackup.h"
#include "CBackupRecorder.h"
// 在CBackup.cpp的顶部添加
#include <filesystem>  // C++17文件系统库，用于create_directories、is_regular_file等
namespace fs = std::filesystem; 


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


bool CopyFileBinary(const std::string& srcPath, const std::string& destPath){
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

// 收集需要备份的文件列表
std::vector<std::string> collectFilesToBackup(const std::string& rootPath, const std::shared_ptr<CConfig>& config) {
    std::vector<std::string> filesList;
    
    // 检查配置是否有效
    if (!config || !fs::exists(rootPath)) {
        return filesList;
    }
    
    // 先根遍历：先添加当前路径
    filesList.push_back(rootPath);
    
    // 如果是目录，递归遍历其子目录和文件
    if (fs::is_directory(rootPath)) {
        // 根据配置决定是否递归遍历
        if (config->isRecursiveSearch()) {
            // 递归遍历目录（先根遍历）
            for (auto it = fs::directory_iterator(rootPath, 
                    fs::directory_options::skip_permission_denied); 
                 it != fs::directory_iterator(); ++it) {
                const auto& p = *it;
                const std::string pathStr = p.path().string();
                
                // 递归调用，继续先根遍历
                std::vector<std::string> subFiles = collectFilesToBackup(pathStr, config);
                filesList.insert(filesList.end(), subFiles.begin(), subFiles.end());
            }
        } else {
            // 非递归遍历，只添加当前目录下的直接子项
            for (auto it = fs::directory_iterator(rootPath, 
                    fs::directory_options::skip_permission_denied); 
                 it != fs::directory_iterator(); ++it) {
                const auto& p = *it;
                const std::string pathStr = p.path().string();
                
                filesList.push_back(pathStr);
            }
        }
    }
    
    return filesList;
}


bool CBackup::doRecovery(const BackupEntry& entry, const std::string& destDir) {
    // 基础恢复：
    // - 若是打包：调用解包器（此处保留输出提示，具体实现按打包器完成）
    // - 若非打包：从备份目录将文件按原始相对路径复制回去

    const std::string backupRoot = entry.destDirectory; // 记录中的目标目录（备份落地位置）
    std::string backupName = entry.backupFileName; // 记录中的备份文件名或相对路径

    const fs::path backupPath = fs::path(backupRoot) / backupName;  
    // 删除的话保留最初的备份文件，形成的中间文件都被删掉
    bool isDecrypted = false;
    bool isDecompressed = false;

    // 先解密

    // 再解压缩
    CompressFactory compressFactory;
    if(compressFactory.isCompressedFile(backupRoot + "/" + backupName)){
        std::cout << "Decompressing file:" << backupName << std::endl;
        // 创建对应类型打包器
        std::string decompressType = compressFactory.getCompressType(backupRoot + "/" + backupName);
        if(decompressType.empty()){
            std::cerr << "Error: Unknown compress type for file: " << backupName << std::endl;
            return false;
        }
        std::unique_ptr<ICompress> decompressor = nullptr;
        try {
            decompressor = compressFactory.createCompress(decompressType);
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to create decompressor: " << e.what() << std::endl;
            return false;
        }
        // 解压缩到备份目录
        // 这里是懒得改了
        std::string sourcePath = backupRoot + "/" + backupName;
        // 将后缀去除
        backupName = backupName.substr(0, backupName.find_last_of('.'));
        if (!decompressor->decompressFile(sourcePath, backupRoot + "/" + backupName)) {
            std::cerr << "Error: Failed to decompress file: " << backupName << std::endl;
            return false;
        }
        // 解压完成了，如果之前有解密过，那就删除解密后的文件
        if(isDecrypted){
            if(!fs::remove(sourcePath)){
                std::cerr << "Error: Failed to remove compressed file: " << sourcePath << std::endl;
                return false;
            }
        }
        isDecompressed = true;
    }


    // 最后解包
    PackFactory packFactory;
    if (packFactory.isPackedFile(backupRoot + "/" + backupName)) {
        std::cout << "Unpacking file: " << backupName << std::endl;
        // 创建对应类型打包器
        std::string packType = packFactory.getPackType(backupRoot + "/" + backupName);
        if(packType.empty()){
            std::cerr << "Error: Unknown pack type for file: " << backupName << std::endl;
            return false;
        }
        std::unique_ptr<IPack> packer = nullptr;
        try {
            packer = PackFactory::createPacker(packType);
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to create packer: " << e.what() << std::endl;
            return false;
        }
        // 解包到源文件目录
        if (!packer->unpack(backupRoot + "/" + backupName, destDir)) {
            std::cerr << "Error: Failed to unpack file: " << backupName << std::endl;
            return false;
        }
        // 解包完成了，如果之前有解密过或者压缩过，那就删除解密后的文件或者压缩后的文件
        if(isDecompressed || isDecrypted){
            if(!fs::remove(backupRoot + "/" + backupName)){
                std::cerr << "Error: Failed to remove packed file: " << backupRoot + "/" + backupName << std::endl;
                return false;
            }
        }
        return true;
    }

    // 非打包：按路径直接复制
    // 入口记录里 sourceFullPath 是原文件应恢复到的绝对路径
    // 这里改为恢复到 destDir 下的相对路径
    const fs::path restorePath = fs::path(destDir) / entry.sourceFullPath.substr(entry.sourceFullPath.find_last_of('/') + 1);
    try {
        fs::create_directories(restorePath.parent_path());
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory for restore: " << e.what() << std::endl;
        return false;
    }

    // 直接复制文件
    try {
        if (!fs::exists(backupPath)) {
            std::cerr << "Error: backup file not found: " << backupPath.string() << std::endl;
            return false;
        }
        fs::copy_file(backupPath, restorePath, fs::copy_options::overwrite_existing);
    } catch (const std::exception& e) {
        std::cerr << "Error restoring file: " << e.what() << std::endl;
        return false;
    }

    return true;
}


std::string CBackup::doBackup(const std::shared_ptr<CConfig>& config) {
    // 1) 基础校验
    if (!config || !config->isValid()) {
        std::cerr << "Error: Invalid backup configuration" << std::endl;
        return "";
    }

    // 2) 准备源集合（优先单源路径，如为空再尝试多源路径）
    std::vector<std::string> sourceRoots;
    if (!config->getSourcePath().empty()) {
        sourceRoots.push_back(config->getSourcePath());
    } else {
        for (const auto& p : config->getSourcePaths()) {
            sourceRoots.push_back(p);
        }
    }
    if (sourceRoots.empty()) {
        std::cerr << "Error: No source specified" << std::endl;
        return "";
    }

    // 3) 收集需要备份的条目（含目录与文件；目录用于创建结构，文件用于拷贝）
    std::vector<std::string> filesToBackup;
    std::string destPath;

    // 理论上只需要设计一个源就好，这个是之前的设计漏洞，后面改进一下
    // TODO
    filesToBackup = collectFilesToBackup(sourceRoots[0], config);

    if (filesToBackup.empty()) {
        std::cerr << "Error: No files to backup" << std::endl;
        return "";
    }

    // 4) 创建目标根目录
    const std::string destinationRoot = config->getDestinationPath();
    try {
        fs::create_directories(destinationRoot);
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to create destination root: " << e.what() << std::endl;
        return "";
    }

    // 5) 是否打包（基础版：若未启用打包，则直接镜像拷贝；启用打包则调用打包器）
    if (config->isPackingEnabled()) {
        std::unique_ptr<IPack> packer = nullptr;
        try {
            packer = PackFactory::createPacker(config->getPackType());
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to create packer: " << e.what() << std::endl;
            return "";
        }

        // 基础实现：将收集的文件直接打包到目标目录下（由具体打包器决定扩展名）
        // 调用打包器打包文件
        const std::string packedFilePath = packer->pack(filesToBackup, destinationRoot);
        destPath = packedFilePath;
        if (packedFilePath.empty()) {
            std::cerr << "Error: Failed to pack files" << std::endl;
            return "";
        }

        // 5.1)  打包完判断是否需要压缩
        if(config->isCompressionEnabled()){
            std::unique_ptr<ICompress> compress = nullptr;
            try{
                compress = CompressFactory::createCompress(config->getCompressionType());
            } catch (const std::exception& e) {
                std::cerr << "Error: Failed to create compress: " << e.what() << std::endl;
                return "";
            }
            // 这个路径需要斟酌一下，或许增加一个后缀，总之是要将之前的文件给覆盖了
            const std::string compressedFilePath = compress->compressFile(packedFilePath);
            destPath = compressedFilePath;
            if(compressedFilePath.empty()){
                std::cerr << "Error: Failed to compress file" << std::endl;
                return "";
            }
            // 要是压缩成功的话就把之前的文件删掉
            if(fs::exists(compressedFilePath)){
                try{
                    fs::remove(packedFilePath);
                } catch (const std::exception& e) {
                    std::cerr << "Error: Failed to remove packed file: " << e.what() << std::endl;
                    return "";
                }
            }
        }

        // TODO: 记录备份信息
        return destPath;
    }

    // 6) 非打包路径：直接拷贝。若是目录，保持相对路径结构拷贝到 destinationRoot
    destPath = destinationRoot;
    for(const auto& root : sourceRoots){
        const fs::path rootPath = fs::path(root).parent_path();
        if (fs::exists(root)) {
            // 遍历所有收集的条目
            for (const auto& entry : filesToBackup) {
                // 只处理以当前root为前缀的条目
                if (entry.rfind(rootPath.string(), 0) == 0) {
                    const std::string relativePath = fs::relative(entry, rootPath).string();
                    // 为空，说明相对路径就是当前目录，直接以entry作为相对路径
                    const std::string destinationPath = (relativePath.empty() ? 
                                                        (fs::path(destinationRoot) / entry).string() : 
                                                        (fs::path(destinationRoot) / relativePath).string());
                    
                    std::cout << "Copying " << entry << " to " << destinationPath << std::endl;
                    std::cout << "Root:" << rootPath << std::endl;
                    std::cout << "Relative Path:" << relativePath << std::endl;

                    try {
                        if (fs::is_directory(entry)) {
                            // 确保目录存在
                            fs::create_directories(destinationPath);
                        } else if (fs::is_regular_file(entry)) {
                            // 确保目标文件的父目录存在
                            fs::create_directories(fs::path(destinationPath).parent_path());
                            // 复制文件
                            CopyFileBinary(entry, destinationPath);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing " << entry << ": " << e.what() << std::endl;
                        return "";
                    }
                }
            }
        }
    }

    return destPath;
}
