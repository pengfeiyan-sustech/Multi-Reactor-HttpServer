# 这是一个 CMakeLists.txt 文件
cmake_minimum_required(VERSION 3.0.0)

project(test)

set(CMAKE_CXX_STANDARD 11)

# 头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件 main.cpp
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# 动态库路径
# link_directories(${PROJECT_SOURCE_DIR}/lib)

# 生成可执行文件
add_executable(app ${SRC_LIST})

# 动态库链接放在最后
target_link_libraries(app -lpthread)