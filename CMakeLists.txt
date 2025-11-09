# 最低CMake版本要求
cmake_minimum_required(VERSION 3.14)

# 项目名称和版本
project(BackupTool VERSION 1.0 LANGUAGES CXX)

# 设置C++标准（全局生效）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 静态运行时库配置（必须在添加子目录之前设置）
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
endif()

# 添加JSON库
include(FetchContent)
# FetchContent_Declare(
#     json
#     GIT_REPOSITORY https://github.com/nlohmann/json.git
#     GIT_TAG v3.11.3  # 指定版本
# )
# FetchContent_MakeAvailable(json)
include_directories(${CMAKE_SOURCE_DIR}/lib/json/include)

# 添加GLFW库
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw ${CMAKE_BINARY_DIR}/glfw)

# 添加Dear ImGui库
file(GLOB IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/lib/imgui/*.cpp"
    "${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends
)
target_link_libraries(imgui PUBLIC glfw)

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# 设置输出目录（全局生效）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含子目录（分别处理主程序和测试）
add_subdirectory(src)    # 主程序逻辑（src/CMakeLists.txt）
add_subdirectory(test)   # 测试逻辑（test/CMakeLists.txt）