set_project("smol-game")
set_version("0.0.1")

add_rules("mode.debug", "mode.release")

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

add_requires("zig 0.15.2", {kind = "toolchain"})

set_toolchains("@zig")
set_languages("cxx20")

includes("smol-engine")

target("smol-game")
    set_kind("binary")
    set_languages("cxx20")
    add_cxflags(
        "-mavx",
        "-mavx2",
        "-mfma",
        "-mbmi",
        "-mbmi2",
        "-mf16c",
        "-mlzcnt",
        "-mpopcnt"
    )
    
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