// Encoding: UTF-8
#ifndef IPACK_H
#define IPACK_H

#include <string>
#include <vector>

// IPack 抽象类 - 文件打包与解包接口
class IPack {
public:
    virtual ~IPack() = default;

    // 打包：输入文件列表，输出打包目标路径（不含扩展名由具体实现决定）
    virtual bool pack(const std::vector<std::string>& files, const std::string& destPath) = 0;

    // 解包：输入打包文件，输出解包目录
    virtual bool unpack(const std::string& srcPath, const std::string& destDir) = 0;
};

#endif