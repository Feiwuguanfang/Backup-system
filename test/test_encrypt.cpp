#include <gtest/gtest.h>

#include "SimpleXOREncrypt.h"  // 包含您的加密功能头文件

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include "testUtils.h"

// 测试用例：测试基本的加密和解密功能
TEST(EncryptionTest, BasicEncryptionDecryption) {
    const std::string sourceFile = "test_source.txt";
    const std::string encryptedFile = "test_encrypted.txt";
    const std::string decryptedFile = "test_decrypted.txt";
    const std::string testContent = "Hello, World!";

    // 测试准备：创建测试文件
    // 创建原始文件
    CreateTestFile(sourceFile, testContent);
    // 创建密钥
    std::string key = "SecretKey123";
    // 创建加密文件
    SimpleXOREncrypt encryptor;
    std::string encryptedPath = encryptor.encryptFile(sourceFile, key);
    // 检查加密文件是否存在
    EXPECT_TRUE(std::filesystem::exists(encryptedPath)) << "Encrypted file not found";
    // 创建解密文件
    bool result = encryptor.decryptFile(encryptedPath, decryptedFile, key);
    // 检查解密文件是否存在
    EXPECT_TRUE(std::filesystem::exists(decryptedFile)) << "Decrypted file not found";
    // 检查解密是否成功
    EXPECT_TRUE(result) << "Decryption failed";
    std::vector<char> decryptedContent;
    // 读取解密文件内容
    EXPECT_TRUE(ReadTestFile(decryptedFile, decryptedContent)) << "Failed to read decrypted file";
    // 检查解密内容是否与原始内容一致
    EXPECT_EQ(std::string(decryptedContent.begin(), decryptedContent.end()), testContent)  << "Decrypted content mismatch";

    // 清理文件
    // 删除测试文件
    CleanupTestFile(sourceFile);
    CleanupTestFile(encryptedPath);
    CleanupTestFile(decryptedFile);
}