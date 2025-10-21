#include <gtest/gtest.h>

#include "HuffmanCompress.h"  // 包含您的压缩功能头文件

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include "testUtils.h"

// 测试用例：测试基本的压缩和解压功能
// 修复基本压缩测试用例
TEST(CompressionTest, BasicCompressionDecompression) {
    const std::string sourceFile = "test_source.txt";
    const std::string decompressedFile = "test_decompressed.txt";
    const std::string testContent = "Hello, this is a test for Huffman compression!";
    
    // 清理可能存在的旧测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(decompressedFile);
    
    // 测试准备：创建测试文件
    ASSERT_TRUE(CreateTestFile(sourceFile, testContent)) << "Failed to create test file";

    // 创建压缩器
    HuffmanCompress huffmanCompressor;
    
    // 执行压缩 - 修改为正确的接口调用
    std::string compressedFile = huffmanCompressor.compressFile(sourceFile);
    ASSERT_FALSE(compressedFile.empty()) << "Compression failed";
    
    // 验证压缩文件存在
    ASSERT_TRUE(std::filesystem::exists(compressedFile)) << "Compressed file not created";
    
    // 执行解压
    bool decompressResult = huffmanCompressor.decompressFile(compressedFile, decompressedFile);
    ASSERT_TRUE(decompressResult) << "Decompression failed";
    
    // 读取解压后的文件内容
    std::vector<char> decompressedContent;
    ASSERT_TRUE(ReadTestFile(decompressedFile, decompressedContent)) << "Failed to read decompressed file";
    
    // 验证解压后的内容与原始内容一致
    ASSERT_EQ(std::string(decompressedContent.begin(), decompressedContent.end()), testContent) 
        << "Decompressed content does not match original content";
    
    // 清理测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(compressedFile);
    CleanupTestFile(decompressedFile);
}

// 同样修改其他测试用例
// 修复空文件测试用例
TEST(CompressionTest, EmptyFile) {
    const std::string sourceFile = "test_empty.txt";
    const std::string decompressedFile = "test_empty_decompressed.txt";
    const std::string emptyContent = "";
    
    // 清理可能存在的旧测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(decompressedFile);
    
    // 测试准备：创建空测试文件
    ASSERT_TRUE(CreateTestFile(sourceFile, emptyContent)) << "Failed to create empty test file";

    // 创建压缩类
    HuffmanCompress huffmanCompressor;
    
    // 执行压缩 - 修改为正确的接口调用
    std::string compressedFile = huffmanCompressor.compressFile(sourceFile);
    ASSERT_FALSE(compressedFile.empty()) << "Compression of empty file failed";
    
    // 执行解压
    bool decompressResult = huffmanCompressor.decompressFile(compressedFile, decompressedFile);
    ASSERT_TRUE(decompressResult) << "Decompression of empty file failed";
    
    // 读取解压后的文件内容
    std::vector<char> decompressedContent;
    ASSERT_TRUE(ReadTestFile(decompressedFile, decompressedContent)) << "Failed to read decompressed empty file";
    
    // 验证解压后的内容是空的
    ASSERT_EQ(decompressedContent.size(), 0) << "Decompressed empty file should be empty";
    
    // 清理测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(compressedFile);
    CleanupTestFile(decompressedFile);
}

// 修复CRC32校验测试用例
TEST(CompressionTest, CRC32Check) {
    const std::string sourceFile = "test_crc.txt";
    const std::string testContent = "CRC32 check test content";
    
    // 清理可能存在的旧测试文件
    CleanupTestFile(sourceFile);
    
    // 测试准备：创建测试文件
    ASSERT_TRUE(CreateTestFile(sourceFile, testContent)) << "Failed to create test file";

    // 创建压缩类
    HuffmanCompress huffmanCompressor;
    
    // 执行压缩 - 修改为正确的接口调用
    std::string compressedFile = huffmanCompressor.compressFile(sourceFile);
    ASSERT_FALSE(compressedFile.empty()) << "Compression failed";
    
    // 清理测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(compressedFile);
}