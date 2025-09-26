# 项目编译

## 编译
1. 确保已安装CMake和C++编译器
2.  在根目录下运行build_project脚本：./build_project.bat
3.  可执行文件将生成在bin目录下


# 测试 
目前仅使用了google test框架，未来可能引入更多
## Google Test

### 部署方式
1. cd lib/googletest
2. mkdir build
3. cd build
4. cmake ..
5. cmake --build .

当前我是已经部署成功，但是可能会出现我本地的构造无法适配其他机器的构造，下面介绍解决方法
### 部署方式
1. 删除lib目录里的googletest，使用以下指令重新构建项目：
    #### 在项目根目录下执行
    git clone https://github.com/google/googletest.git lib/googletest
    cd lib/googletest
    mkdir build
    cd build
    cmake ..
    cmake --build .


# 项目输出
项目的输出将在bin目录下生成可执行文件，项目可执行文件名为backup_app.exe， 测试可执行文件名为test_app.exe。
*注意*： 项目可执行文件的入口是src/main.cpp，而测试可执行文件的入口是test/test_main.cpp，所以如果要进行改动的话需要找对文件，测试文件的输出结果会出现一个说明表格。



# 版本
| 版本 | 日期 | 说明 |
| --- | --- | --- |
| 0.1 | 2025-09-26 | 完成基本框架搭建，为项目添加了google test框架 |