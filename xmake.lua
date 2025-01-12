add_rules("mode.debug", "mode.release")
set_languages("c++20")
add_toolchains("clang")
add_requires("glfw", "glm","vulkan")

target("vulkantest")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    set_targetdir("$(projectdir)/bin")
    add_packages("glfw", "glm","vulkan")
    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})

    
    if is_mode("release") then 
        add_defines("NDEBUG")
    end
    
    after_build(function(target)
        try{
            function ()
                os.cd("$(projectdir)/shader")
                os.exec("bash compile.sh")
            end,
            catch{
                function (errors)
                    print("error: " .. errors)
                end
            }
        }
    end)
