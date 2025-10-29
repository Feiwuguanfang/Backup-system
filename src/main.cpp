#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "CBackup.h"
#include "CConfig.h"
#include "PackFactory.h"
#include "CompressFactory.h"
#include "CBackupRecorder.h"

namespace fs = std::filesystem;

// 全局备份仓库路径配置
static std::string BACKUP_REPOSITORY_ROOT = ".\\backup_repository";

//--mode backup --src "F:\courser_project\software_development\testBox\ori_A" --dst "./"
//--mode recover --dst "./" --to "./restore_repository/ori_A"

static void printHelp(){
    std::cout << "Usage (pseudo CLI):\n"
              << "--mode backup  --src <path> --dst <relative_path> [--include \".*\\.txt\"  --pack <packType>(default: none)  --compress <compressType>(default: none)]\n"
              << "--mode recover --dst <relative_path> --to <target_path>\n";
}


static std::vector<std::string> tokenize(const std::string& line){
    std::vector<std::string> tokens;
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') { inQuotes = !inQuotes; continue; }
        if (!inQuotes && std::isspace(static_cast<unsigned char>(c))) {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

static int runParsed(const std::vector<std::string>& args){
    std::string mode;
    std::string packType = "none";  // 新增加一个参数用于指定打包算法,默认不打包
    std::string compressType = "none";  // 新增加一个参数用于指定压缩算法,默认不压缩
    std::string srcPath;
    std::string dstPath;
    std::string includeRegex;
    std::string restoreTo;
    std::string repoPath;

    auto nextVal = [&](size_t& i, std::string& out){ if (i + 1 < args.size()) { out = args[++i]; return true; } return false; };
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--mode") { nextVal(i, mode); }
        else if (arg == "--pack") { nextVal(i, packType); }
        else if (arg == "--compress") { nextVal(i, compressType); }
        else if (arg == "--src") { nextVal(i, srcPath); }
        else if (arg == "--dst") { nextVal(i, dstPath); }
        else if (arg == "--include") { nextVal(i, includeRegex); }
        else if (arg == "--to") { nextVal(i, restoreTo); }
        else if (arg == "--repo") { nextVal(i, repoPath); }
        else if (arg == "--help" || arg == "-h") { printHelp(); return 0; }
    }
    
    // 设置备份仓库路径
    if (!repoPath.empty()) {
        BACKUP_REPOSITORY_ROOT = repoPath;
        std::cout << "Backup repository set to: " << BACKUP_REPOSITORY_ROOT << std::endl;
        return 0;
    }

    // 创建备份记录器
    CBackupRecorder backupRecorder(BACKUP_REPOSITORY_ROOT);

    if (mode == "backup") {
        if (srcPath.empty() || dstPath.empty()) { printHelp(); return 1; }
        
        // 计算实际备份路径：仓库路径 + 相对路径 
        std::string actualBackupPath = BACKUP_REPOSITORY_ROOT;
        if (!dstPath.empty()) {
            fs::path dst = fs::path(dstPath);
            actualBackupPath = fs::absolute(fs::path(BACKUP_REPOSITORY_ROOT) / dst).string();
        }

        std::cout << "Source: " << srcPath << std::endl;
        std::cout << "Relative path: " << dstPath << std::endl;
        std::cout << "Actual backup path: " << actualBackupPath << std::endl;
        
        // 将这里的路径设置为绝对路径
        auto config = std::make_shared<CConfig>();
        config->setSourcePath(fs::absolute(fs::path(srcPath)).string())
              .setDestinationPath(fs::absolute(fs::path(actualBackupPath)).string())
              .setRecursiveSearch(true);

        // 判断是否需要打包
        if(packType != "none"){
            // 检查指定的打包器类型是否支持
            if(!PackFactory::isPackTypeSupported(packType)){
                std::cerr << "Error: Packing algorithm type " << packType << " is not supported.\n";
                return 1;
            }
            else{
                // 设置打包器类型
                config->setPackType(packType)
                        .setPackingEnabled(true);

                // 判断是否需要压缩
                if(compressType != "none"){
                    // 检查指定的压缩器类型是否支持
                    if(!CompressFactory::isCompressTypeSupported(compressType)){
                        std::cerr << "Error: Compress algorithm type " << compressType << " is not supported.\n";
                        return 1;
                    }
                    else{
                        // 设置压缩器类型
                        config->setCompressionType(compressType)
                                .setCompressionEnabled(true);
                    }
                }
            }
        }

        // 备份执行
        if (!includeRegex.empty()) config->addIncludePattern(includeRegex);
        CBackup backup;
        std::string destPath = backup.doBackup(config);
        if (destPath.empty()) { std::cerr << "Backup failed" << std::endl; return 2; }
        std::cout << "Backup finished -> " << destPath << std::endl;
        // 备份记录
        backupRecorder.addBackupRecord(config, destPath);
        return 0;
    } else if (mode == "recover") {
        if (dstPath.empty() || restoreTo.empty()) { printHelp(); return 1; }
        // 把目标路径转换为绝对路径
        restoreTo = fs::absolute(fs::path(restoreTo)).string();
        
        // 这个时候就查看有没有备份记录，如果是空的，就提示用户没有备份记录
        if(backupRecorder.getBackupRecords().empty()){
            std::cerr << "Error: No backup records found. Please run backup first.\n";
            return 1;
        }

        // 查找备份记录中是否有匹配的文件名
        auto records = backupRecorder.findBackupRecordsByFileName(dstPath);
        if(records.empty()){
            std::cerr << "Error: No backup records found for file " << dstPath << ". Please check the filename.\n";
            return 1;
        }

        // 可能有多个，需要让用户来选择
        BackupEntry entry = records[0]; // 目前先默认选择第一个，后续可以改进为交互选择
            
        // 执行恢复
        CBackup backup;
        bool success = backup.doRecovery(entry, restoreTo);
        
        if (!success) { 
            std::cerr << "Recovery failed" << std::endl; 
            return 2; 
        }
        std::cout << "Recovery finished -> " << restoreTo << std::endl;
        return 0;
    }
    printHelp();
    return 1;
}

int main(){
    std::cout << "Backup interactive CLI. Type 'help' for usage, 'demo' for example, 'exit' to quit." << std::endl;
    printHelp();
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        // 去掉首尾空白
        auto trim = [](std::string s){ size_t a = s.find_first_not_of(" \t\r\n"); size_t b = s.find_last_not_of(" \t\r\n"); if (a==std::string::npos) return std::string(); return s.substr(a, b-a+1); };
        line = trim(line);
        if (line.empty()) continue;
        if (line == "exit" || line == "quit") break;
        if (line == "help" || line == "--help" || line == "-h") { printHelp(); continue; }
        auto tokens = tokenize(line);
        if (tokens.empty()) continue;
        runParsed(tokens);
    }
    return 0;
}