#ifndef ICOMPRESS_H
#define ICOMPRESS_H

#include <string>
#include <vector>
#include <memory>

// ICompress抽象类
class ICompress {
public:
    // 虚析构函数
    virtual ~ICompress() = default;
};

#endif // ICOMPRESS_H