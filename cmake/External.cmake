set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/third_party/_deps")
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm_cache")
include(CPM)
set(VK_SDK_VERSION 1.4.304)

set(ANA_PATCH_DIR ${CMAKE_SOURCE_DIR}/patches)

CPMAddPackage(
    NAME backward
    GITHUB_REPOSITORY bombela/backward-cpp
    GIT_TAG v1.6
)

CPMAddPackage(
    NAME unordered_dense
    GITHUB_REPOSITORY martinus/unordered_dense
    VERSION 4.1.2
)

# CPMAddPackage(
# NAME Format.cmake
# VERSION 1.8.3
# GITHUB_REPOSITORY TheLartians/Format.cmake
# OPTIONS

# # set to yes skip cmake formatting
# "FORMAT_SKIP_CMAKE YES"

# # set to yes skip clang formatting
# "FORMAT_SKIP_CLANG NO"

# # path to exclude (optional, supports regular expressions)
# "CMAKE_FORMAT_EXCLUDE cmake/CPM.cmake"
# )

# CPMAddPackage(
# NAME tinygltf
# GITHUB_REPOSITORY syoyo/tinygltf
# VERSION 2.8.18
# PATCHES ${ANA_PATCH_DIR}/tinygltf.patch
# )

# if(APH_ENABLE_TRACING)
# CPMAddPackage(
# NAME tracy
# GITHUB_REPOSITORY wolfpld/tracy
# VERSION 0.11.1
# )
# endif()
find_program(PATCH_EXECUTABLE patch)

if(NOT PATCH_EXECUTABLE)
    message(FATAL_ERROR "patch command not found")
endif()

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
    OPTIONS
    "GLM_TEST_ENABLE OFF"
    PATCH_COMMAND ${PATCH_EXECUTABLE} -p1 -N -i ${ANA_PATCH_DIR}/glm.patch -d ${glm_SOURCE_DIR}
)

CPMAddPackage(
    NAME vma
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG master
)

# CPMAddPackage(
# NAME vulkan-headers
# GITHUB_REPOSITORY KhronosGroup/Vulkan-Headers
# GIT_TAG vulkan-sdk-${VK_SDK_VERSION}
# DOWNLOAD_ONLY YES
# )
# add_library(vulkan-registry INTERFACE IMPORTED)
# target_include_directories(vulkan-registry INTERFACE ${vulkan-headers_SOURCE_DIR}/include)
# target_compile_definitions(vulkan-registry INTERFACE
# VULKAN_HPP_NO_EXCEPTIONS
# VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
# )

# CPMAddPackage(
# NAME GLFW
# GITHUB_REPOSITORY glfw/glfw
# GIT_TAG 3.3.9
# OPTIONS
# "GLFW_BUILD_TESTS OFF"
# "GLFW_BUILD_EXAMPLES OFF"
# "GLFW_BULID_DOCS OFF"
# "GLFW_INSTALL OFF"
# )

# CPMAddPackage(
# NAME imgui
# GITHUB_REPOSITORY ocornut/imgui
# GIT_TAG docking
# DOWNLOAD_ONLY YES
# )

# add_library(imgui STATIC
# ${imgui_SOURCE_DIR}/imgui.cpp
# ${imgui_SOURCE_DIR}/imgui_demo.cpp
# ${imgui_SOURCE_DIR}/imgui_draw.cpp
# ${imgui_SOURCE_DIR}/imgui_tables.cpp
# ${imgui_SOURCE_DIR}/imgui_widgets.cpp
# ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
# ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
# )
# target_include_directories(imgui PUBLIC
# ${imgui_SOURCE_DIR}
# ${imgui_SOURCE_DIR}/backends
# )
# target_link_libraries(imgui PRIVATE glfw)
