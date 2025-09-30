#ifndef CFILEFILTER_H
#define CFILEFILTER_H
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include "CConfig.h"

// 这个是一个附加功能 先把打包解包实现了先

// CFileFilter类 - 负责文件筛选功能  主要可能有根据文件类型 大小 时间 正则表达式等condition进行筛选  最终需要得到一个筛选后的文件列表
// 输入的就是CConfig对应的配置信息  输出的就是筛选后的文件列表
// 还有可能是用户 组的排除
class CFileFilter {
public:
    // 构造函数
    CFileFilter();

    // 目录遍历   in: 目录路径（因为过滤配置也在config中，就干脆直接输入config）   out: 文件列表  --> 这里可以加上过滤操作   --> 先根遍历
    
    std::vector<std::string> filterFiles(const CConfig& config);
};


#endif
