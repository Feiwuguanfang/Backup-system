# include "myPack.h"

std::string myPack::pack(const std::vector<std::string>& files, const std::string& destPath) {
    std::vector<FileMeta> metas;
    // 包头长度 = 是否打包（1字节） + 算法类型(1字节) + 文件数量(4字节)
    size_t headerLen = 1 + 1 + 4;

    // 元数据区长度
    size_t metaLen = 0;
    for(const auto& file : files){
        metaLen += 4; // 文件名长度
        metaLen += file.size(); // 文件名内容
        metaLen += 8; // 文件大小
        metaLen += 8; // 文件偏移量
    }
    size_t contentStart = headerLen + metaLen;

    // 计算每个文件的 offset
    uint64_t currentOffset = contentStart;

    // 收集各个文件的元数据
    for(const auto& file : files){
        // 尝试读取文件，判断文件是否存在
        if(!std::filesystem::exists(file)){
            std::cerr << "Error: File " << file << " does not exist.\n";
            return "";
        }

        uint64_t size = std::filesystem::file_size(file);
        // 记录文件名长度
        uint32_t nameLen = file.size();
        metas.push_back({nameLen, file, size, currentOffset});
        currentOffset += size;
    }

    const std::string baseName = "backup_" + std::to_string(time(nullptr)) + "." + getPackTypeName();
    const std::string destPackBase = (std::filesystem::path(destPath) / baseName).string();

    // 接下来写入包头（包括打包算法，当前包包含的文件数量，文件的元信息）
    std::ofstream out(destPackBase, std::ios::binary);
    if(!out){
        std::cerr << "Error: Failed to open file " << destPackBase << " for writing.\n";
        return "";
    }
    // 写入是否打包（1字节）
    uint8_t isPacked = 1;
    out.write(reinterpret_cast<const char*>(&isPacked), sizeof(isPacked));

    // 写入打包算法（1字节）
    PackType type = PackType::Basic;
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // 写入当前包包含的文件数量（4字节）
    uint32_t fileCount = metas.size();
    out.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

    // 写入文件元信息
    for(const auto& meta : metas){
        out.write(reinterpret_cast<const char*>(&meta.nameLen), sizeof(meta.nameLen));
        out.write(meta.name.c_str(), meta.nameLen);
        out.write(reinterpret_cast<const char*>(&meta.size), sizeof(meta.size));
        out.write(reinterpret_cast<const char*>(&meta.offset), sizeof(meta.offset));
    }

    // 写入文件内容（按顺序排列）
    for(const auto& meta : metas){
        std::ifstream in(meta.name, std::ios::binary);
        if(!in){
            std::cerr << "Error: Failed to open file " << meta.name << " for reading.\n";
            return "";
        }
        std::vector<char> buffer(meta.size);
        in.read(buffer.data(), meta.size);
        out.write(buffer.data(), meta.size);
    }

    out.close();
    std::cout << "Packing " << files.size() << " files to " << destPackBase << " using " << getPackTypeName() << "Packer.\n";
    return destPackBase;
}

bool myPack::unpack(const std::string& srcPath, const std::string& destDir) {
    std::ifstream in(srcPath, std::ios::binary);
    if(!in){
        std::cerr << "Error: Failed to open file " << srcPath << " for reading.\n";
        return false;
    }

    // 检查是否是打包文件
    uint8_t isPacked;
    in.read(reinterpret_cast<char*>(&isPacked), sizeof(isPacked));
    if(isPacked != 1){
        // 不是打包文件，返回错误信息
        std::cerr << "Error: File " << srcPath << " is not packed.\n";
        return false;
    }

    // 读取包头
    // 读取打包算法类型(1字节)
    PackType type;
    in.read(reinterpret_cast<char*>(&type), sizeof(type));
    if(type != PackType::Basic){
        // 匹配失败，返回错误信息
        std::cerr << "Error: Packing algorithm type in " << srcPath << " is not Basic.\n";
        return false;
    }

    // 读取文件数量（4字节）
    uint32_t fileCount;
    in.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    std::vector<FileMeta> metas(fileCount);

    // 读取文件元信息
    for(auto& meta : metas){
        in.read(reinterpret_cast<char*>(&meta.nameLen), sizeof(meta.nameLen));
        meta.name.resize(meta.nameLen);
        in.read(&meta.name[0], meta.nameLen);
        in.read(reinterpret_cast<char*>(&meta.size), sizeof(meta.size));
        in.read(reinterpret_cast<char*>(&meta.offset), sizeof(meta.offset));
    }
    
    // 依次读取文件，写入目标文件夹
    for(const auto& meta : metas){
        // 定义到对应的文件内容offset（必须是当前流位置开始，也就是元数据区末尾）
        in.seekg(meta.offset, std::ios::beg);
        // 读取文件
        std::vector<char> buffer(meta.size);
        in.read(buffer.data(), meta.size);

        // 写入目标目录
        std::filesystem::path outPath = meta.name;
        std::ofstream out(outPath, std::ios::binary);
        if(!out){
            std::cerr << "Error: Failed to open file " << outPath << " for writing.\n";
            return false;
        }
        out.write(buffer.data(), meta.size);
        out.close();
    }

    in.close();
    std::cout << "Unpacking " << fileCount << " files from " << srcPath << " to " << destDir << " using BasicPacker.\n";
    return true;
}

