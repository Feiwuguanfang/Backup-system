#ifndef MYCOMPRESS_H
#define MYCOMPRESS_H

#include "ICompress.h"

// 自定义压缩算法实现类（继承自ICompress接口）
class MyCompress : public ICompress {
public:
    MyCompress();  // 构造函数声明
    virtual ~MyCompress() = default;  // 虚析构函数

    // 实现ICompress接口的所有纯虚函数
    bool compressFile(const std::string& sourcePath, const std::string& destPath) override;
    bool decompressFile(const std::string& sourcePath, const std::string& destPath) override;
    bool compressData(const std::vector<char>& sourceData, std::vector<char>& destData) override;
    bool decompressData(const std::vector<char>& sourceData, std::vector<char>& destData) override;
    void setCompressionLevel(int level) override;
    std::string getFileExtension() const override;
    
private:
    int m_compressionLevel; // 压缩级别（1-9）
};

#endif // MYCOMPRESS_H