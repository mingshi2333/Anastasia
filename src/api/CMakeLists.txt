file(GLOB API_VULKAN_SRC *.cpp vulkan/*.cpp)

ana_setup_target(api ${API_VULKAN_SRC})

target_link_libraries(ana-api PUBLIC Vulkan::Vulkan)