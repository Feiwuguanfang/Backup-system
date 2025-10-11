#ifndef MYPACK_H
#define MYPACK_H

#include "IPack.h"
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>


// 定义元数据结构
struct FileMeta{
    uint32_t nameLen;
    std::string name;
    uint64_t size;
    uint64_t offset;
};

/*
 * @brief 基础打包器类，实现基本的文件打包与解包功能。
 * @description 打包文件格式为：
 *  1. 打包标志位（1字节），固定为0x01
 *  2. 打包算法（1字节）
 *  3. 当前包包含的文件数量（4字节）
 *  4. 文件元信息 : 文件名长度（4字节） 文件名(变长) 文件大小（8字节） 偏移量（8字节）
 *  5. 文件内容（按顺序排列）
*/


class myPack : public IPack {
public:
    bool myPack::pack(const std::vector<std::string>& files, const std::string& destPath) override;

    bool myPack::unpack(const std::string& srcPath, const std::string& destDir) override;
};


#endif