#ifndef CBACKUPRECORDER_H
#define CBACKUPRECORDER_H
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 定义结构体，用于记录备份记录
struct BackupEntry{
    std::string fileName;   // 源文件名
    std::string destPath;   // 备份文件路径
    std::string backupTime; // 备份时间
    std::string checksum;   // 文件校验和
};

// 为 BackupEntry 提供 nlohmann/json 所需的序列化支持
namespace nlohmann {
    template <>
    struct adl_serializer<BackupEntry> {
        static void to_json(json& j, const BackupEntry& entry) {
            j = json{{"file_name", entry.fileName},
                     {"destination_path", entry.destPath},
                     {"backup_time", entry.backupTime},
                     {"checksum", entry.checksum}};
        }

        static void from_json(const json& j, BackupEntry& entry) {
            j.at("file_name").get_to(entry.fileName);
            j.at("destination_path").get_to(entry.destPath);
            j.at("backup_time").get_to(entry.backupTime);
            j.at("checksum").get_to(entry.checksum);
        }
    };
}

// 重载相等运算符，用于比较两个BackupEntry对象
inline bool operator==(const BackupEntry& lhs, const BackupEntry& rhs) {
    return lhs.fileName == rhs.fileName && lhs.backupTime == rhs.backupTime;
}


class CBackupRecorder{
public:
    CBackupRecorder();
    ~CBackupRecorder();

    // 从文件中加载备份目录（这里假定程序有一个固定的备份记录文件）
    bool loadBackupRecordsFromFile(const std::string& filePath);

    // 保存备份记录
    bool saveBackupRecordsToFile(const std::string& filePath);

    // 添加备份记录
    void addBackupRecord(const BackupEntry& entry);

    // 获取备份记录
    const std::vector<BackupEntry>& getBackupRecords() const;

    // 根据文件名查找备份记录
    std::vector<BackupEntry> findBackupRecordsByFileName(const std::string& queryFileName) const;

    // 根据备份时间查找条目
    std::vector<BackupEntry> findBackupRecordsByBackupTime(const std::string& startime, const std::string& endTime) const;

    // 获取备份条目的全局索引
    size_t getBackupRecordIndex(const BackupEntry& entry) const;

    // 根据索引删除备份记录
    bool deleteBackupRecord(size_t index);

    // 根据备份记录删除条目
    bool deleteBackupRecord(const BackupEntry& entry);

    // 修改备份记录
    bool modifyBackupRecord(size_t index, const BackupEntry& newEntry);

     // 检查索引是否有效
    bool isIndexValid(size_t index) const;

    bool modifyBackupRecord(const BackupEntry& oldEntry, const BackupEntry& newEntry);


private:
    std::vector<BackupEntry> backupRecords; // 备份记录容器
};


#endif