cmake_minimum_required(VERSION 3.20)
project(Anastasia)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ANA_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ANA_ROOT_DIR}/cmake")

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/cmake/modules")
include(CompilerOptions) # Find dependencies
include(External)

# RHI library
add_library(ana_rhi INTERFACE)
target_include_directories(ana_rhi INTERFACE 
    ${PROJECT_SOURCE_DIR}/src/rhi/include
)

# Vulkan backend for RHI
add_library(ana_vk STATIC
    ${PROJECT_SOURCE_DIR}/src/vk/src/vk_device.cpp
)
target_include_directories(ana_vk 
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/vk/include
)
# The vk backend will implement the rhi api
target_link_libraries(ana_vk PUBLIC ana_rhi)

# Find all source files
file(GLOB_RECURSE ALL_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Exclude files that are part of our libraries from the main executable sources
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*/(vk/src|api/vulkan)/.*")

# Add executable
add_executable(Anastasia ${ALL_SOURCE_FILES})

# Include directories for the main executable
target_include_directories(Anastasia PRIVATE
    include
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(Anastasia
    PRIVATE
    ana_vk # Link against the RHI implementation
    glfw
    Threads::Threads
    glm::glm
    GPUOpen::VulkanMemoryAllocator
    imgui
)

# Compile definitions
target_compile_definitions(Anastasia PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

add_custom_command(
    TARGET Anastasia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_SOURCE_DIR}/shaders sh compile.sh
    COMMENT "Compiling shaders..."
    VERBATIM
)
