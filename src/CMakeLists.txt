# 收集主程序源文件（排除main.cpp）
file(GLOB_RECURSE SOURCES "*.cpp")  # 业务逻辑代码
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")  # 排除main.cpp
file(GLOB_RECURSE HEADERS "../include/*.h")            # 头文件

# 创建核心库（业务逻辑封装）
add_library(backup_core STATIC ${SOURCES} ${HEADERS})

# 核心库配置（包含目录、编译选项等）
target_include_directories(backup_core PUBLIC ../include)
if(MSVC)
  target_compile_options(backup_core PRIVATE /W4 /utf-8 /Zc:__cplusplus)
else()
  target_compile_options(backup_core PRIVATE -Wall -Wextra -Wpedantic)
endif()


# 主应用程序（仅包含入口文件main.cpp）
add_executable(backup_app main.cpp)
target_link_libraries(backup_app PRIVATE backup_core)  # 链接核心库

# 链接GUI库 (GLFW + Dear ImGui)
target_include_directories(backup_app PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends
)
target_link_libraries(backup_app PRIVATE imgui glfw)

# 链接OpenGL库
if(WIN32)
    # Windows平台需要链接OpenGL库
    target_link_libraries(backup_app PRIVATE opengl32)
elseif(APPLE)
    # macOS平台
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(backup_app PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
else()
    # Linux平台
    find_package(OpenGL REQUIRED)
    target_link_libraries(backup_app PRIVATE OpenGL::GL)
endif()

if(MSVC)
  target_compile_options(backup_app PRIVATE /utf-8 /Zc:__cplusplus)
endif()