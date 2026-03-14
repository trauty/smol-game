set_project("smol-game")
set_version("0.0.1")

add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

set_languages("cxx20")

if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
end 

includes("smol-engine")

target("smol-game")
    set_kind("binary")
    set_languages("cxx20")
    add_cxflags("-march=x86-64-v3")

    if is_mode("release") then
        set_optimize("fastest")
        set_strip("all")
    end
    
    add_deps("smol-engine")

    add_files("src/**.cpp")
    add_includedirs("src")

    if is_plat("windows") then
        if is_mode("release") then
            add_ldflags("-mwindows", {force = true})
        end
    elseif is_plat("linux") then 
        add_ldflags("-Xlinker --allow-shlib-undefined")
    end

    after_build(function (target)
        local dest_dir = target:targetdir()
        os.trycp("assets", dest_dir)
    end)
target_end()