#include "CompressFactory.h"
#include "MyCompress.h"  // 包含具体压缩类的头文件
#include <stdexcept>

// 根据压缩类型创建实例（工厂模式核心逻辑）
std::unique_ptr<ICompress> CompressFactory::createCompress(const std::string& compressType) {
    if (compressType == "myCompress") {
        return std::make_unique<MyCompress>();  // 创建MyCompress实例
    }
    // 后续可添加其他压缩类型（如"gzip"、"zip"）
    // else if (compressType == "gzip") {
    //     return std::make_unique<GzipCompress>();
    // }
    else {
        throw std::runtime_error("Unknown compress type: " + compressType);
    }
}