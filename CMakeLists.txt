cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
project(Anastasia)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
    ANA_ROOT_DIR
    ${CMAKE_SOURCE_DIR}
)
set(
    ANA_BUILD_DIR
    ${CMAKE_BINARY_DIR}
)
set(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "${ANA_ROOT_DIR}/cmake"
)
set(
    ANA_EXAMPLES_DIR
    ${APH_ROOT_DIR}/examples
)
set(
    ANA_SRC_DIR
    ${ANA_ROOT_DIR}/src
)
set(
    ANA_OUTPUT_DIR
    ${ANA_BUILD_DIR}/bin
)

include(AnaHelper)

ana_option(ANA_SHARED "Enable building shared library" OFF)
ana_option(ANA_ENABLE_TESTING "Enable testing" OFF)
ana_option(ANA_ENABLE_TRACING "Enable tracer" OFF)
ana_option(ANA_ENABLE_TSAN "Enable thread sanitizer" OFF)
ana_option(ANA_ENABLE_ASAN "Enable address sanitizer" OFF)
ana_option(ANA_ENABLE_MSAN "Enable memory sanitizer" OFF)

include(CompilerOptions) # Find dependencies
include(External)
include(AnaCompilerOptions)

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

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src/api)
add_subdirectory(src/math)
add_subdirectory(src/camera)

target_link_libraries(Anastasia
    PRIVATE
    glfw
    Threads::Threads
    glm::glm
    ana-api
    ana-math
    ana-camera
    GPUOpen::VulkanMemoryAllocator
    backward
    dw
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
