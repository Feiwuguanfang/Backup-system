#include "CompressFactory.h"
#include <stdexcept>

// 根据压缩类型创建实例（工厂模式核心逻辑）
std::unique_ptr<ICompress> CompressFactory::createCompress(const std::string& compressType) {
    if(!isCompressTypeSupported(compressType)){
        throw std::runtime_error("Unknown compress type: " + compressType);
    }
    if (compressType == "Huffman") {
        return std::make_unique<HuffmanCompress>();  // 创建HuffmanCompress实例
    }
    // 后续继续补充
    return nullptr;
}

std::vector<std::string> CompressFactory::getSupportedCompressTypes() {
    // 后续继续补充
    return {"Huffman"};
}


bool CompressFactory::isCompressTypeSupported(const std::string& compressType) {
    // 后续继续添加
    return compressType == "Huffman";
}

std::string CompressFactory::getCompressType(const std::string& filePath) {
    // 根据文件的第二个字节判断压缩类型，第一个字节适用于判断当前文件是否为压缩文件的标志位
    std::ifstream in(filePath, std::ios::binary);
    if(!in){
        std::cerr << "Error: Failed to open file " << filePath << " for reading.\n";
        return "";
    }
    // 跳过第一个字节
    in.seekg(1, std::ios::beg);
    CompressType type;
    in.read(reinterpret_cast<char*>(&type), sizeof(type));
    if(type == CompressType::Huffman){
        return "Huffman";
    }
    // 后续继续补充
    return "";
}

bool CompressFactory::isCompressedFile(const std::string& filePath) {
    // 应该是第一个位既是标志位，第二个字节也是符合条件的压缩类型
    if(!std::filesystem::exists(filePath)){
        std::cerr << "Error: File " << filePath << " does not exist.\n";
        return false;
    }
    std::ifstream in(filePath, std::ios::binary);
    if(!in){
        std::cerr << "Error: Failed to open file " << filePath << " for reading.\n";
        return false;
    }
    uint8_t firstByte;
    in.read(reinterpret_cast<char*>(&firstByte), sizeof(firstByte));
    return firstByte == 0x01 && getCompressType(filePath) != "";
}