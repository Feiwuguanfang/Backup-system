#ifndef MYCOMPRESS_H
#define MYCOMPRESS_H

#include "ICompress.h"

class MyCompress : public ICompress{
public:
    
    virtual ~MyCompress() = default;
    // 压缩:将文件经过压缩到指定目录
    bool doCompress(const std::string& sourcePath, const std::string& destPath);
    // 解压:将文件经过解压到指定目录
    bool doUnCompress(const std::string& sourcePath, const std::string& destPath);
};

#endif