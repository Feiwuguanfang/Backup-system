#include "PackFactory.h"
#include <iostream>

// 基础空实现的打包器：仅占位，返回成功/失败标志
class BasicPacker : public IPack {
public:
    bool pack(const std::vector<std::string>& /*files*/, const std::string& /*destPath*/) override {
        // 占位实现：直接提示未真正实现，返回false表示未打包
        std::cout << "[BasicPacker] pack() not implemented, skip.\n";
        return false;
    }

    bool unpack(const std::string& /*srcPath*/, const std::string& /*destDir*/) override {
        std::cout << "[BasicPacker] unpack() not implemented, skip.\n";
        return false;
    }
};

std::unique_ptr<IPack> PackFactory::createPacker(const std::string& /*packType*/){
    // 目前统一返回占位实现，避免链接错误。
    return std::make_unique<BasicPacker>();
}

std::vector<std::string> PackFactory::getSupportedPackTypes(){
    return {"basic"};
}

bool PackFactory::isPackTypeSupported(const std::string& packType){
    return packType == "basic";
}


