@echo off
setlocal

rem 检查build目录是否存在，不存在则创建
if not exist "build" mkdir build
cd build

rem 执行CMake生成项目
cmake ..
if %ERRORLEVEL% NEQ 0 (
echo CMake generate failed, please check the error message
pause
exit /b %ERRORLEVEL%
)

rem 编译项目
cmake --build . --config Debug
if %ERRORLEVEL% NEQ 0 (
echo Compile failed, please check the error messages
pause
exit /b %ERRORLEVEL%
)

echo Compile success, the executable file is in bin directory
pause
endlocal