#ifndef IPACK_H
#define IPACK_H

#include <string>
#include <vector>
#include <memory>

// IPack抽象类 - 负责文件打包和解包功能
class IPack {
public:
    // 虚析构函数
    virtual ~IPack() = default;

    // 打包    in: 文件列表    out: 打包文件
    virtual bool pack(const std::vector<std::string>& files, const std::string& destPath) = 0;

    // 解包    in: 打包文件  解包目录     out: 解包文件
    virtual bool unpack(const std::string& srcPath, const std::string& destDir) = 0;

    // 还有多种文件类型该如何处理  普通文件 目录 链接文件  管道文件 
};

#endif // IPACK_H