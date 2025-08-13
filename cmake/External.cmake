include(CPM)
set(VK_SDK_VERSION 1.4.304)

set(ANA_PATCH_DIR ${CMAKE_SOURCE_DIR}/patches)

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
CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
    OPTIONS
    "GLM_TEST_ENABLE OFF"
    PATCHES ${ANA_PATCH_DIR}/glm.patch
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

# #### <<<<<<< imgui
# CPMAddPackage(
# NAME imgui
# VERSION 1.91.8
# URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8-docking.tar.gz
# DOWNLOAD_ONLY YES
# )

# add_library(imgui STATIC
# ${imgui_SOURCE_DIR}/imgui.cpp
# ${imgui_SOURCE_DIR}/imgui_demo.cpp
# ${imgui_SOURCE_DIR}/imgui_draw.cpp
# ${imgui_SOURCE_DIR}/imgui_tables.cpp
# ${imgui_SOURCE_DIR}/imgui_widgets.cpp
# ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp

# $<$<BOOL:${APH_WSI_BACKEND_IS_GLFW}>:${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp>
# $<$<BOOL:${APH_WSI_BACKEND_IS_SDL2}>:${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp>
# )
# target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
# target_link_libraries(imgui
# PRIVATE
# $<$<BOOL:${APH_WSI_BACKEND_IS_GLFW}>:glfw>
# $<$<BOOL:${APH_WSI_BACKEND_IS_SDL2}>:SDL2::SDL2-static>
# )
# target_compile_definitions(imgui PRIVATE VK_NO_PROTOTYPES)