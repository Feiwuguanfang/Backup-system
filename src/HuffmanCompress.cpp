#include "HuffmanCompress.h"
#include <cstdint>

// CRC32查找表（预计算）
static constexpr uint32_t crc32Table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

// 计算CRC32校验值
static uint32_t calculateCRC32(const std::vector<uint8_t>& data, uint32_t crc = 0xFFFFFFFF) {
    for (uint8_t byte : data) {
        crc = crc32Table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

namespace fs = std::filesystem;

bool HuffmanCompress::readFreqTable(const std::string& sourcePath, std::array<uint64_t, 256>& freqTable, uint64_t& originalSize){
    freqTable.fill(0);
    originalSize = 0;
    // 读取文件
    std::ifstream in(sourcePath, std::ios::binary);
    if(!in || !in.is_open()){
        std::cerr << "Error: Failed to open file " << sourcePath << " for reading.\n";
        return false;
    }

    // 统计词频
    std::vector<uint8_t> buffer(BUFF_SIZE);
    while(in){
        // 每次读取一个buffer的大小
        in.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(BUFF_SIZE));
        std::streamsize n = in.gcount();    //  可能没有满一个buffer的空间，需要查看实际读取的字节数
        if(n <= 0)  break;
        originalSize += static_cast<uint64_t>(n);
        // 统计词频
        for(std::streamsize i = 0;i < n; ++ i){
            ++freqTable[buffer[i]];
        }
    }
    return true;
}


HNode* HuffmanCompress::buildHuffmanTree(const std::array<uint64_t, 256>& freqTable){
    // 通过优先队列构造小顶堆
    std::priority_queue<HNode*, std::vector<HNode*>, HNodeCmp> pq;
    for(int i = 0;i < 256; i++){
        if(freqTable[i] > 0){
            pq.push(new HNode(freqTable[i], static_cast<uint8_t>(i)));
        }
    }

    // 特殊情况，队列为空
    if(pq.empty()) return new HNode(0, 0);

    // 特殊情况，队列只有一个符号
    // 但是不能直接返回，至少要有一个深度，得到一位的编码
    if(pq.size() == 1){
        HNode* only = pq.top(); pq.pop();
        HNode* fake = new HNode(0, 0);
        HNode* root = new HNode(only->freq + fake->freq, 0, only, fake);
        return root;
    }

    // 正常合并
    while(pq.size() > 1){
        HNode* a = pq.top(); pq.pop();
        HNode* b = pq.top(); pq.pop();
        HNode* parent = new HNode(a->freq + b->freq, 0, a, b);
        pq.push(parent);
    }
    return pq.top();
}

void HuffmanCompress::generateHuffmanCodes(HNode* node, std::vector<bool>& cur, std::array<std::vector<bool>, 256>& codes){
    if(!node) return;
    if(node->isLeaf()){
        // 将编码填入对应编码表中
        if(cur.empty()){
            codes[node->byte] = std::vector<bool>{false};
        }else{
            codes[node->byte] = cur;
        }
        return ;
    }
    // 左边是0
    cur.push_back(false);
    generateHuffmanCodes(node->left, cur, codes);
    cur.pop_back(); // 回溯

    // 右边是1
    cur.push_back(true);
    generateHuffmanCodes(node->right, cur, codes);
    cur.pop_back();
}

std::array<std::vector<bool>, 256> HuffmanCompress::generateHuffmanCodes(HNode* root){
    std::array<std::vector<bool>, 256> codes;
    std::vector<bool> cur;
    generateHuffmanCodes(root, cur, codes);
    return codes;
}


std::string HuffmanCompress::compressFile(const std::string& sourcePath){
    // 检查目标文件路径是否可以访问
    // 直接检查是不是存在不太对，因为还没创建一定不存在
    // TODO: 检查目标文件路径是否可以访问

    // 读取内容统计每个字节的出现频率
    std::array<uint64_t, 256> freq;
    uint64_t originalSize = 0;
    if(!readFreqTable(sourcePath, freq, originalSize)){
        std::cerr << "Error: Failed to read frequency table from file " << sourcePath << ".\n";
        return "";
    }
    // 构造哈夫曼树
    HNode* root = buildHuffmanTree(freq);
    if(!root){
        std::cerr << "Error: Failed to build Huffman tree.\n";
        return "";
    }

    // 生成编码表
    auto codes = generateHuffmanCodes(root);
    // 释放资源
    deleteHuffmanTree(root);
    if(codes.empty()){
        std::cerr << "Error: Failed to generate Huffman codes.\n";
        return "";
    }
    
    // 随后创建文件头，存储CRC offset 压缩算法类型
    // 在原先文件基础上增加后缀即可
    std::string destPath = sourcePath + ".huff";
    std::ofstream out(destPath, std::ios::binary);
    if(!out|| !out.is_open()){
        std::cerr << "Error: Failed to open file " << destPath << " for writing.\n";
        return "";
    }

    // 计算词频表大小
    uint32_t freqTableSize = 0;
    for(int i = 0;i < 256; i++){
        if(freq[i] > 0){
            freqTableSize += 1 + 8; // 1字节字节值 + 8字节频率
        }
    }

    // 创建文件头信息
    Head header;
    header.isCompress = 1;
    header.compressType = CompressType::Huffman;
    header.validBits = 0;
    header.reservedBits = 0;
    header.headerSize = sizeof(Head);
    header.freqTableSize = freqTableSize;
    header.originalSize = originalSize;
    header.crc32 = 0; // 先设为0，后续计算

    // 写入文件头
    out.write(reinterpret_cast<const char*>(&header), sizeof(Head));

    // 之后将词频表内容写入文件头
    for(int i = 0 ;i < 256; i++){
        if(freq[i] > 0){
            out.write(reinterpret_cast<const char*>(&i), 1);
            out.write(reinterpret_cast<const char*>(&freq[i]), 8);
        }
    }

    // 将内容写入文件中
    // 打开文件
    std::ifstream in(sourcePath, std::ios::binary);
    if(!in || !in.is_open()){
        std::cerr << "Error: Failed to open file " << sourcePath << " for reading.\n";
        return "";
    }

    uint32_t crcValue = 0xFFFFFFFF;

    // 准备buffer（压缩缓冲区）
    std::vector<uint8_t> readBuffer(BUFF_SIZE);
    std::vector<uint8_t> writeBuffer(BUFF_SIZE);
    std::vector<bool> bitBuffer;
    size_t writePos = 0;
    uint8_t currentByte = 0;
    int bitPosition = 0;


    // 辅助函数：将当前字节写入输出缓冲区
    auto flushCurrentByte = [&]() {
        if (writePos >= writeBuffer.size()) {
        // 输出缓冲区已满，写入文件
        out.write(reinterpret_cast<const char*>(writeBuffer.data()), writePos);
        writePos = 0;
        }
        writeBuffer[writePos++] = currentByte;
        currentByte = 0;
        bitPosition = 0;
    };


    // 压缩
    while(in){
        // 读取一块数据
        in.read(reinterpret_cast<char*>(readBuffer.data()), static_cast<std::streamsize>(BUFF_SIZE));
        std::streamsize n = in.gcount();
        if(n <= 0)  break;

        // 计算CRC
        for(std::streamsize i = 0; i < n; i++){
            crcValue = crc32Table[(crcValue ^ readBuffer[i]) & 0xFF] ^ (crcValue >> 8);
        }

        // 压缩数据
        for(std::streamsize i = 0;i < n; i++){
            uint8_t byte = readBuffer[i];
            const std::vector<bool>& code = codes[byte];

            for(bool bit : code){
                // 使用位运算将对应位进行填充
                currentByte |= (bit << (7 - bitPosition));
                bitPosition++;

                if(bitPosition == 8){
                    // 满了8字节，写入输出缓冲区
                    flushCurrentByte();
                }
            }
        }
    }
    // 有可能最后没满1字节
    if(bitPosition > 0){
        header.validBits = 8 - bitPosition;
        // 写入最后一个字节
        flushCurrentByte();
    }

    // 写入剩余的缓冲区内容
    if (writePos > 0) {
        out.write(reinterpret_cast<const char*>(writeBuffer.data()), writePos);
    }

    header.crc32 = crcValue ^ 0xFFFFFFFF;

    // 回写文件头
    out.seekp(0, std::ios::beg);
    out.write(reinterpret_cast<const char*>(&header), sizeof(Head));
    out.close();
    in.close();
    return destPath;
}

bool HuffmanCompress::decompressFile(const std::string& sourcePath, const std::string& destPath){
    // 打开压缩文件
     std::ifstream in(sourcePath, std::ios::binary);
    if(!in || !in.is_open()){
        std::cerr << "Error: Failed to open file " << sourcePath << " for reading.\n";
        return false;
    }

    // 打开目标文件写入
    std::ofstream out(destPath, std::ios::binary);
    if(!out || !out.is_open()){
        std::cerr << "Error: Failed to open file " << destPath << " for writing.\n";
        in.close();
        return false;
    }

    // 读取头信息
    Head header;
    in.read(reinterpret_cast<char*>(&header), sizeof(Head));

    // 验证是否位压缩文件或压缩类型
    if(header.isCompress != 1 || header.compressType != CompressType::Huffman){
        std::cerr << "Error: File " << sourcePath << " is not a Huffman compressed file.\n";
        in.close();
        out.close();
        return false;
    }

    // 读取词频表
    std::array<uint64_t, 256> freqTable = {0};
    for(uint32_t i = 0 ;i < header.freqTableSize;){
        uint8_t byte;
        uint64_t freq;
        in.read(reinterpret_cast<char*>(&byte), 1);
        in.read(reinterpret_cast<char*>(&freq), 8);
        freqTable[byte] = freq;
        i += 1 + 8;
    }

    // 重建 Huffman 树
    HNode* root = buildHuffmanTree(freqTable);
    if(!root){
        std::cerr << "Error: Failed to build Huffman tree.\n";
        in.close();
        out.close();
        return false;
    }

    // 准备一块缓存区
    std::vector<uint8_t> readBuffer(BUFF_SIZE);
    std::vector<uint8_t> decompressedData;
    decompressedData.reserve(header.originalSize);

    // 进行解压
    HNode* currentNode = root;
    uint64_t decompressedCount = 0;
    bool isLastByte = false;

    while(in && decompressedCount < header.originalSize){
        // 读取一块数据
        in.read(reinterpret_cast<char*>(readBuffer.data()), static_cast<std::streamsize>(BUFF_SIZE));
        std::streamsize n = in.gcount();
        if(n <= 0)  break;
        
        // 遍历读取的数据
        for(std::streamsize i = 0;i < n && decompressedCount < header.originalSize; i++){
            uint8_t byte = readBuffer[i];
            int bitsToProcess = 8;

            // 检查是不是最后一个字节
            if(i == n - 1 && !in.peek()){
                isLastByte = true;
                bitsToProcess -= header.validBits;
            }

            for(int j = 0;j < bitsToProcess && decompressedCount < header.originalSize; j++){
                // 获取当前位的值
                bool bit = (byte >> (7 - j)) & 1;

                // 沿着huffman树移动
                if(bit){
                    // 1就右边
                    currentNode = currentNode->right;
                }else{
                    currentNode = currentNode->left;
                }
                // 到了叶子节点，输出对应字节
                if(currentNode->isLeaf()){
                    decompressedData.push_back(currentNode->byte);
                    decompressedCount++;
                    currentNode = root; // 重置节点
                }
            }
        }
    }

    // 释放资源
    deleteHuffmanTree(root);

    // 计算校验码
    uint32_t calculatedCRC = 0xFFFFFFFF;
    for(uint8_t byte : decompressedData){
        calculatedCRC = crc32Table[(calculatedCRC ^ byte) & 0xFF] ^ (calculatedCRC >> 8);
    }
    calculatedCRC ^= 0xFFFFFFFF;

    // 校验校验码
    if(calculatedCRC != header.crc32){
        std::cerr << "Error: CRC32 checksum mismatch. Decompressed data may be corrupted.\n";
        out.close();
        in.close();
        return false;
    }

    // 将解压后的数据写入目标文件
    out.write(reinterpret_cast<const char*>(decompressedData.data()), decompressedData.size());


    out.close();
    in.close();
    return true;
}


