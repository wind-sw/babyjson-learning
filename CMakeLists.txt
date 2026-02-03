cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(BabyJSON LANGUAGES CXX)

# C++17 标准设置（关键补全）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)      # 强制使用 C++17，不允许回退
set(CMAKE_CXX_EXTENSIONS OFF)            # 禁用编译器扩展，确保跨平台一致性

# 创建可执行文件
add_executable(main main.cpp)

# 头文件搜索路径（如果你有 print.h 等头文件）
target_include_directories(main PRIVATE include)

# 编译警告选项（推荐添加）
if(MSVC)
    target_compile_options(main PRIVATE /W4 /permissive-)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()