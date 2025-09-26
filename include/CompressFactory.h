#ifndef COMPRESSFACTORY_H
#define COMPRESSFACTORY_H

#include "ICompress.h"
#include <memory>
#include <string>


// 工厂类，用于创造不一样的压缩类
class CompressFactory{
public:
    static std::unique_ptr<ICompress> createCompress(const std::string& compressType);
};

#endif
