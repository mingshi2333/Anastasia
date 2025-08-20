# API module CMakeLists.txt

# Collect all source files in this directory
file(GLOB API_VULKAN_SRC "*.cpp" ,"*.h")

set(TARGET api)

# Create a library from the sources
add_library(api STATIC ${API_VULKAN_SRC})

target_link_libraries(api PUBLIC Vulkan::Vulkan)