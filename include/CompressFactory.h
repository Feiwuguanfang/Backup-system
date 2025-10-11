// Encoding: UTF-8
#ifndef COMPRESSFACTORY_H
#define COMPRESSFACTORY_H

#include <string>
#include <memory>
#include "ICompress.h"  // 依赖ICompress抽象类

// 压缩工厂类：负责责创建不同类型的压缩器实例
class CompressFactory {
public:
    // 根据静态压缩缩类型创建对应的压缩器（返回ICompress智能指针）
    static std::unique_ptr<ICompress> createCompress(const std::string& compressType);
};

#endif // COMPRESSFACTORY_H