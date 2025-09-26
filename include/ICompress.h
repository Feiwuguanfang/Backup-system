# ifndef ICOMPRESS_H
# define ICOMPRESS_H

#include <string>
#include <vector>


// ICompress抽象类
class ICompress{
public:
    // 虚函数
    virtual ~ICompress() = default;

    // 压缩:将文件经过压缩到指定目录
    virtual bool doCompress(const std::string& sourcePath, const std::string& destPath);
    
    // 解压:将文件经过解压到指定目录
    virtual bool doUnCompressck(const std::string& sourcePath, const std::string& destPath);
};


# endif