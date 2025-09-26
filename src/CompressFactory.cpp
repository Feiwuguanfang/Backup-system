#include "CompressFactory.h"
#include "myCompress.h"
#include <stdexcept>

std::unique_ptr<ICompress> CompressFactory::createCompress(const std::string& compressType){
    if(compressType == "myCompress"){
        return std::make_unique<MyCompress>();
    }
    // 后续添加
    // else if(){

    // }
    throw std::runtime_error("Unknown compress type");
}
