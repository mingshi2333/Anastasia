add_rules("mode.debug", "mode.release")
-- add_rules("utils.install.pkgconfig_importfiles")
-- add_rules("utils.install.cmake_importfiles")
set_languages("c++20")
add_toolchains("clang")
add_requires("glfw", "glm","vulkan")



target("vulkantest")
    set_kind("binary")
    add_files("src/*.cpp")
    --add_includedirs("$(projectdir)/shader",{public=true})
    set_targetdir("$(projectdir)")
    --set_targetdir("$(projectdir)")
    add_packages("glfw", "glm","vulkan")
    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
    if is_mode("release") then 
        add_defines("NDEBUG")
    end
