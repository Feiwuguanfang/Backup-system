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

# 可能问题
Q：还原是不是可以指定还原路径比较好？
A：


# 版本
| 版本 | 日期 | 说明 |
| --- | --- | --- |
| 0.1 | 2025-09-26 | 完成基本框架搭建，为项目添加了google test框架 |
| 0.2 | 2025-09-27 | 完成了最基本的备份功能，同时为了方便后续的还原功能的开发，增设备份记录类，并成功通过测试（同时增加了json的外部库）(这里因为还是在实现基础功能，就没有开新的分支) |
| 0.3 | 2025-10-07 | 备份功能的完善，还原功能的实现，编译后运行生成的backup_app.exe程序，命令行操作进行备份和还原。进行备份时使用--mode backup --src "源路径" --dst "备份仓库相对路径"，可选添加--include "正则表达式"来筛选特定类型的文件。恢复时使用--mode recover --dst "备份仓库相对路径" --to "目标还原路径"命令。可通过--repo "路径"命令设置备份仓库位置|
| 0.4 | 2025-10-11 | 实现了基本的打包和解包算法，接下来将进一步结合压缩进行实现|