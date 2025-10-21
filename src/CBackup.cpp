#include <vector>
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



bool CBackup::doRecovery(const BackupEntry& entry) {
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
        if (!packer->unpack(backupRoot + "/" + backupName, entry.sourceFullPath)) {
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
    const fs::path restorePath = fs::path(entry.sourceFullPath);
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


bool CBackup::doBackup(const std::shared_ptr<CConfig>& config) {
    // 1) 基础校验
    if (!config || !config->isValid()) {
        std::cerr << "Error: Invalid backup configuration" << std::endl;
        return false;
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
        return false;
    }

    // 3) 收集需要备份的条目（含目录与文件；目录用于创建结构，文件用于拷贝）
    std::vector<std::string> filesToBackup;
    std::vector<std::string> dirsToCreate;

    for (const auto& root : sourceRoots) {
        if (fs::is_regular_file(root)) {
            if (config->shouldIncludeFile(root)) {
                filesToBackup.push_back(root);
            }
        } else if (fs::is_directory(root)) {
            // 目录先记录根目录（用于创建）
            dirsToCreate.push_back(root);
            // 递归遍历 - 先根遍历
            for (auto it = fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied); it != fs::recursive_directory_iterator(); ++it) {
                const auto& p = *it;
                const std::string pathStr = p.path().string();
                if (p.is_directory()) {
                    dirsToCreate.push_back(pathStr);
                } else if (p.is_regular_file()) {
                    if (config->shouldIncludeFile(pathStr)) {
                        filesToBackup.push_back(pathStr);
                    }
                }
            }
        } else {
            std::cerr << "Warning: Source not file or directory: " << root << std::endl;
        }
    }

    if (filesToBackup.empty() && dirsToCreate.empty()) {
        std::cerr << "Error: No files to backup" << std::endl;
        return false;
    }

    // 4) 创建目标根目录
    const std::string destinationRoot = config->getDestinationPath();
    try {
        fs::create_directories(destinationRoot);
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to create destination root: " << e.what() << std::endl;
        return false;
    }

    // 5) 是否打包（基础版：若未启用打包，则直接镜像拷贝；启用打包则调用打包器）
    if (config->isPackingEnabled()) {
        std::unique_ptr<IPack> packer = nullptr;
        try {
            packer = PackFactory::createPacker(config->getPackType());
        } catch (const std::exception& e) {
            std::cerr << "Error: Failed to create packer: " << e.what() << std::endl;
            return false;
        }

        // 基础实现：将收集的文件直接打包到目标目录下（由具体打包器决定扩展名）
        // 调用打包器打包文件
        const std::string packedFilePath = packer->pack(filesToBackup, destinationRoot);
        if (packedFilePath.empty()) {
            std::cerr << "Error: Failed to pack files" << std::endl;
            return false;
        }

        // 5.1)  打包完判断是否需要压缩
        if(config->isCompressionEnabled()){
            std::unique_ptr<ICompress> compress = nullptr;
            try{
                compress = CompressFactory::createCompress(config->getCompressionType());
            } catch (const std::exception& e) {
                std::cerr << "Error: Failed to create compress: " << e.what() << std::endl;
                return false;
            }
            // 这个路径需要斟酌一下，或许增加一个后缀，总之是要将之前的文件给覆盖了
            const std::string compressedFilePath = compress->compressFile(packedFilePath);
            if(compressedFilePath.empty()){
                std::cerr << "Error: Failed to compress file" << std::endl;
                return false;
            }
            // 要是压缩成功的话就把之前的文件删掉
            if(fs::exists(compressedFilePath)){
                try{
                    fs::remove(packedFilePath);
                } catch (const std::exception& e) {
                    std::cerr << "Error: Failed to remove packed file: " << e.what() << std::endl;
                    return false;
                }
            }
        }

        // TODO: 记录备份信息


        return true;
    }

    // 6) 非打包路径：直接拷贝。若是目录，保持相对路径结构拷贝到 destinationRoot
    bool success = true;
    for (const auto& root : sourceRoots) {
        if (fs::is_regular_file(root)) {
            // 单文件：复制到目标根目录，文件名不变
            try {
                const std::string destFilePath = (fs::path(destinationRoot) / fs::path(root).filename()).string();
                fs::create_directories(fs::path(destFilePath).parent_path());
                fs::copy_file(root, destFilePath, fs::copy_options::overwrite_existing);
            } catch (const std::exception& e) {
                std::cerr << "Error copying file " << root << ": " << e.what() << std::endl;
                success = false;
            }
        } else if (fs::is_directory(root)) {
            // 先创建所有需要的目录（保持结构）
            for (const auto& d : dirsToCreate) {
                if (d.rfind(root, 0) == 0) { // 以 root 为前缀
                    const std::string relativePath = fs::relative(d, root).string();
                    const std::string destDirPath = (relativePath.empty() ? destinationRoot : (fs::path(destinationRoot) / relativePath).string());
                    try {
                        fs::create_directories(destDirPath);
                    } catch (const std::exception& e) {
                        std::cerr << "Error creating directory " << destDirPath << ": " << e.what() << std::endl;
                        success = false;
                    }
                }
            }

            // 再复制文件
            for (const auto& f : filesToBackup) {
                if (f.rfind(root, 0) == 0) { // 以 root 为前缀
                    const std::string relativePath = fs::relative(f, root).string();
                    const std::string destFilePath = (fs::path(destinationRoot) / relativePath).string();
                    try {
                        fs::create_directories(fs::path(destFilePath).parent_path());
                        CopyFileBinary(f, destFilePath);
                    } catch (const std::exception& e) {
                        std::cerr << "Error copying file " << f << ": " << e.what() << std::endl;
                        success = false;
                    }
                }
            }
        }
    }

    return success;
}
