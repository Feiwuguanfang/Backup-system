#ifndef PACKFACTORY_H
#define PACKFACTORY_H

#include <string>
#include <memory>
#include "IPack.h"

// 打包器工厂类 - 负责创建不同类型的打包器实例
class PackFactory {
public:
    // 创建打包器的静态方法
    // packType: 打包类型标识，如"zip"、"tar"、"basic"等
    // 返回: 对应类型的IPack接口实例智能指针
    static std::unique_ptr<IPack> createPacker(const std::string& packType);
    
    // 获取所有支持的打包器类型
    static std::vector<std::string> getSupportedPackTypes();
    
    // 检查指定打包类型是否支持
    static bool isPackTypeSupported(const std::string& packType);
};

#endif