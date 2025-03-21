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

# Recursively find all cpp files
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Add executable, using the collected source file list
add_executable(Anastasia ${SOURCE_FILES})

# Include directories
target_include_directories(Anastasia PRIVATE
    include
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/api
)

find_package(Threads REQUIRED)

target_link_libraries(Anastasia
    PRIVATE
    glfw
    Threads::Threads
    glm::glm
    api
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