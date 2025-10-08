#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "CBackup.h"
#include "CConfig.h"

namespace fs = std::filesystem;

static bool writeTextFile(const std::string& path, const std::string& content) {
    try {
        fs::create_directories(fs::path(path).parent_path());
        std::ofstream out(path, std::ios::binary);
        if (!out) return false;
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        return true;
    } catch (...) {
        return false;
    }
}

static bool readFileAll(const std::string& path, std::vector<char>& buf) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    in.seekg(0, std::ifstream::end);
    const auto sz = in.tellg();
    in.seekg(0);
    buf.resize(static_cast<size_t>(sz));
    return static_cast<bool>(in.read(buf.data(), sz));
}

// 全局备份仓库路径配置
static std::string BACKUP_REPOSITORY_ROOT = "./backup_repository";

//--mode backup --src "F:\courser_project\software_development\testBox\ori_A" --dst "./"
//--mode recover --dst "./" --to "./restore_repository/ori_A"

static void printHelp(){
    std::cout << "Usage (pseudo CLI):\n"
              << "--mode backup --src <path> --dst <relative_path> [--include \".*\\.txt\"]\n"
              << "--mode recover --dst <relative_path> --to <target_path>\n";
}

// 目录恢复函数：递归恢复整个目录结构
static bool recoverDirectory(CBackup& backup, const std::string& backupRoot, 
                           const std::string& backupDirPath, const std::string& targetPath) {
    try {
        // 创建目标目录
        fs::create_directories(targetPath);
        
        // 遍历备份目录中的所有文件
        std::string fullBackupDirPath = (fs::path(backupRoot) / backupDirPath).string();
        for (auto it = fs::recursive_directory_iterator(fullBackupDirPath); 
             it != fs::recursive_directory_iterator(); ++it) {
            
            if (it->is_regular_file()) {
                // 计算相对路径
                std::string relativePath = fs::relative(it->path(), fullBackupDirPath).string();
                std::string targetFilePath = (fs::path(targetPath) / relativePath).string();
                
                // 创建目标文件的父目录
                fs::create_directories(fs::path(targetFilePath).parent_path());
                
                // 恢复单个文件
                BackupEntry entry;
                entry.fileName = it->path().filename().string();
                entry.sourceFullPath = targetFilePath;
                entry.destDirectory = backupRoot;
                entry.backupFileName = (fs::path(backupDirPath) / relativePath).string();
                entry.backupTime = "";
                entry.isEncrypted = false;
                entry.isPacked = false;
                entry.isCompressed = false;
                
                if (!backup.doRecovery(entry)) {
                    std::cerr << "Failed to recover file: " << relativePath << std::endl;
                    return false;
                }
                
                std::cout << "  Recovered: " << relativePath << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error during directory recovery: " << e.what() << std::endl;
        return false;
    }
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
    std::string srcPath;
    std::string dstPath;
    std::string includeRegex;
    std::string restoreTo;
    std::string repoPath;

    auto nextVal = [&](size_t& i, std::string& out){ if (i + 1 < args.size()) { out = args[++i]; return true; } return false; };
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--mode") { nextVal(i, mode); }
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

    if (mode == "backup") {
        if (srcPath.empty() || dstPath.empty()) { printHelp(); return 1; }
        
        // 计算实际备份路径：仓库路径 + 相对路径 + 源文件夹名（如果是目录）
        std::string actualBackupPath = BACKUP_REPOSITORY_ROOT;
        if (!dstPath.empty()) {
            actualBackupPath = (fs::path(BACKUP_REPOSITORY_ROOT) / dstPath).string();
        }
        
        // 如果源路径是目录，添加源目录名
        if (fs::is_directory(srcPath)) {
            std::string sourceDirName = fs::path(srcPath).filename().string();
            actualBackupPath = (fs::path(actualBackupPath) / sourceDirName).string();
        }
        
        std::cout << "Source: " << srcPath << std::endl;
        std::cout << "Relative path: " << dstPath << std::endl;
        std::cout << "Actual backup path: " << actualBackupPath << std::endl;
        
        auto config = std::make_shared<CConfig>();
        config->setSourcePath(srcPath)
              .setDestinationPath(actualBackupPath)
              .setRecursiveSearch(true);
        if (!includeRegex.empty()) config->addIncludePattern(includeRegex);
        CBackup backup;
        if (!backup.doBackup(config)) { std::cerr << "Backup failed" << std::endl; return 2; }
        std::cout << "Backup finished -> " << actualBackupPath << std::endl;
        return 0;
    } else if (mode == "recover") {
        if (dstPath.empty() || restoreTo.empty()) { printHelp(); return 1; }
        
        // 计算实际备份路径：仓库路径 + 相对路径
        std::string actualBackupPath = (fs::path(BACKUP_REPOSITORY_ROOT) / dstPath).string();
        
        std::cout << "Relative path: " << dstPath << std::endl;
        std::cout << "Actual backup path: " << actualBackupPath << std::endl;
        
        // 智能恢复：支持文件和目录恢复
        fs::path targetPath(restoreTo);
        std::string targetName = targetPath.filename().string();
        
        // 收集备份目录中的所有文件和目录信息
        std::vector<std::string> allFiles;
        std::vector<std::string> allDirs;
        try {
            for (auto it = fs::recursive_directory_iterator(actualBackupPath); it != fs::recursive_directory_iterator(); ++it) {
                std::string relativePath = fs::relative(it->path(), actualBackupPath).string();
                if (it->is_regular_file()) {
                    allFiles.push_back(relativePath);
                } else if (it->is_directory()) {
                    allDirs.push_back(relativePath);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error searching backup directory: " << e.what() << std::endl;
            return 2;
        }
        
        // 检查是否要恢复整个目录
        bool isDirectoryRecovery = false;
        std::string foundBackupPath;
        
        // 首先检查是否是目录恢复（目标名称在备份目录中作为目录存在）
        for (const auto& dir : allDirs) {
            if (fs::path(dir).filename().string() == targetName) {
                isDirectoryRecovery = true;
                foundBackupPath = dir;
                break;
            }
        }
        
        // 如果不是目录恢复，检查是否是文件恢复
        if (!isDirectoryRecovery) {
            for (const auto& file : allFiles) {
                if (fs::path(file).filename().string() == targetName) {
                    foundBackupPath = file;
                    break;
                }
            }
        }
        
        if (foundBackupPath.empty()) {
            std::cerr << "Error: '" << targetName << "' not found in backup directory" << std::endl;
            std::cerr << "Available items in backup directory:" << std::endl;
            std::cerr << "Files:" << std::endl;
            for (const auto& file : allFiles) {
                std::cerr << "  " << file << std::endl;
            }
            std::cerr << "Directories:" << std::endl;
            for (const auto& dir : allDirs) {
                std::cerr << "  " << dir << std::endl;
            }
            return 2;
        }
        
        // 执行恢复
        CBackup backup;
        bool success = false;
        
        if (isDirectoryRecovery) {
            // 目录恢复：恢复整个目录结构
            std::cout << "Recovering directory: " << targetName << std::endl;
            success = recoverDirectory(backup, actualBackupPath, foundBackupPath, restoreTo);
        } else {
            // 文件恢复：恢复单个文件
            std::cout << "Recovering file: " << targetName << std::endl;
            BackupEntry entry;
            entry.fileName = targetName;
            entry.sourceFullPath = restoreTo;
            entry.destDirectory = actualBackupPath;
            entry.backupFileName = foundBackupPath;
            entry.backupTime = "";
            entry.isEncrypted = false;
            entry.isPacked = false;
            entry.isCompressed = false;
            
            success = backup.doRecovery(entry);
        }
        
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

static int runDemo(){
    std::cout << "[Demo] Backup Repository & Recover flow" << std::endl;
    std::cout << "Creating demo test files and directories..." << std::endl;
    
    const std::string demoRoot = "demo_src";
    const std::string fileA = (fs::path(demoRoot) / "fileA.txt").string();
    const std::string subDir = (fs::path(demoRoot) / "sub").string();
    const std::string fileB = (fs::path(subDir) / "fileB.txt").string();
    const std::string fileC = (fs::path(subDir) / "fileC.txt").string();
    const std::string restoreRoot = "demo_restore";
    
    // 清理旧文件（如果存在）
    std::cout << "Cleaning up previous demo files..." << std::endl;
    fs::remove_all(demoRoot); 
    fs::remove_all(BACKUP_REPOSITORY_ROOT); 
    fs::remove_all(restoreRoot);
    
    // 创建测试文件结构
    std::cout << "Creating demo source files..." << std::endl;
    writeTextFile(fileA, "Hello Backup A\n"); 
    fs::create_directories(subDir); 
    writeTextFile(fileB, "Hello Backup B\n");
    writeTextFile(fileC, "Hello Backup C\n");
    std::cout << "Demo files created successfully!" << std::endl;
    
    // 执行备份（使用相对路径）
    std::string relativePath = "test_backup";
    std::string actualBackupPath = (fs::path(BACKUP_REPOSITORY_ROOT) / relativePath / "demo_src").string();
    
    std::cout << "Repository: " << BACKUP_REPOSITORY_ROOT << std::endl;
    std::cout << "Relative path: " << relativePath << std::endl;
    std::cout << "Actual backup path: " << actualBackupPath << std::endl;
    
    auto config = std::make_shared<CConfig>();
    config->setSourcePath(demoRoot).setDestinationPath(actualBackupPath).setRecursiveSearch(true);
    CBackup backup;
    if (!backup.doBackup(config)) { std::cerr << "Backup failed" << std::endl; return 1; }
    std::cout << "Backup finished -> " << actualBackupPath << std::endl;
    
    // 测试文件恢复
    std::cout << "\n=== Testing File Recovery ===" << std::endl;
    const std::string restoreFile = (fs::path(restoreRoot) / "fileA.txt").string();
    BackupEntry entry; 
    entry.fileName = "fileA.txt"; 
    entry.sourceFullPath = restoreFile; 
    entry.destDirectory = actualBackupPath; 
    entry.backupFileName = "fileA.txt"; 
    entry.backupTime = ""; 
    entry.isEncrypted = false; 
    entry.isPacked = false; 
    entry.isCompressed = false;
    
    if (!backup.doRecovery(entry)) { std::cerr << "File recovery failed" << std::endl; return 2; }
    std::cout << "File recovery finished -> " << restoreFile << std::endl;
    
    // 验证文件恢复
    std::vector<char> aBuf, rBuf; 
    if (!readFileAll(fileA, aBuf) || !readFileAll(restoreFile, rBuf) || aBuf != rBuf) { 
        std::cerr << "File verify failed: restored file mismatch" << std::endl; 
        return 3; 
    }
    std::cout << "File verify OK" << std::endl;
    
    // 测试目录恢复
    std::cout << "\n=== Testing Directory Recovery ===" << std::endl;
    const std::string restoreDir = (fs::path(restoreRoot) / "sub").string();
    if (!recoverDirectory(backup, actualBackupPath, "sub", restoreDir)) {
        std::cerr << "Directory recovery failed" << std::endl;
        return 4;
    }
    std::cout << "Directory recovery finished -> " << restoreDir << std::endl;
    
    // 验证目录恢复
    const std::string restoredFileB = (fs::path(restoreDir) / "fileB.txt").string();
    const std::string restoredFileC = (fs::path(restoreDir) / "fileC.txt").string();
    
    std::vector<char> bBuf, rbBuf, cBuf, rcBuf;
    if (!readFileAll(fileB, bBuf) || !readFileAll(restoredFileB, rbBuf) || bBuf != rbBuf) {
        std::cerr << "Directory verify failed: fileB mismatch" << std::endl;
        return 5;
    }
    if (!readFileAll(fileC, cBuf) || !readFileAll(restoredFileC, rcBuf) || cBuf != rcBuf) {
        std::cerr << "Directory verify failed: fileC mismatch" << std::endl;
        return 6;
    }
    std::cout << "Directory verify OK" << std::endl;
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    std::cout << "Backup repository structure:" << std::endl;
    std::cout << "  " << BACKUP_REPOSITORY_ROOT << "/" << relativePath << "/demo_src/" << std::endl;
    std::cout << "\nDemo completed. Demo files will be cleaned up on next demo run." << std::endl;
    return 0;
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
        if (line == "demo") { runDemo(); continue; }
        auto tokens = tokenize(line);
        if (tokens.empty()) continue;
        runParsed(tokens);
    }
    return 0;
}