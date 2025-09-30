#ifndef MYCOMPRESS_H
#define MYCOMPRESS_H

#include "ICompress.h"

class MyCompress : public ICompress {
public:
    MyCompress();
    virtual ~MyCompress() = default;
    
    // 实现ICompress接口的方法
    virtual bool compressFile(const std::string& sourcePath, const std::string& destPath) override;
    virtual bool decompressFile(const std::string& sourcePath, const std::string& destPath) override;
    virtual bool compressData(const std::vector<char>& sourceData, std::vector<char>& destData) override;
    virtual bool decompressData(const std::vector<char>& sourceData, std::vector<char>& destData) override;
    virtual void setCompressionLevel(int level) override;
    virtual std::string getFileExtension() const override;
    
private:
    int m_compressionLevel; // 压缩级别
};

#endif