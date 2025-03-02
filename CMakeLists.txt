cmake_minimum_required(VERSION 3.20)
project(vulkantest)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# 查找依赖包
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

# GLM是纯头文件库，只需包含路径
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 如果系统安装了glm，可以改用：
# find_package(glm REQUIRED)

# 添加可执行文件
add_executable(vulkantest
    src/main.cpp src/helpfunc.cpp

    # 其他源文件...ls
)

# 包含目录
target_include_directories(vulkantest PRIVATE
    include
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(vulkantest
    PRIVATE
    glfw
    Vulkan::Vulkan
    Threads::Threads
    glm
)

# 编译定义
target_compile_definitions(vulkantest PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Shader编译自定义命令
add_custom_command(
    TARGET vulkantest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_SOURCE_DIR}/shader sh compile.sh
    COMMENT "Compiling shaders..."
    VERBATIM
)