set_project("smol-game")
set_version("0.0.1")

set_config("game_name", "smol-game")
set_config("game_lib_name", "smol-game-logic")

if is_plat("linux") then 
    set_toolchains("clang")
    add_cxflags("-fno-rtti", {force = true})
elseif is_plat("windows") then
    set_toolchains("clang-cl")
    set_toolset("ld", "lld-link")
    set_toolset("sh", "lld-link")
    set_toolset("ar", "llvm-ar")

    add_cxflags("/GR-", {force = true})
    set_runtimes("MD")
end

set_languages("cxx20")
add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

if is_mode("debug") then
    set_policy("build.sanitizer.address", true)
end 

includes("smol-engine")

target("smol-game")
    set_kind("shared")
    set_basename(get_config("game_lib_name"))
    add_cxflags("-march=x86-64-v3")

    add_defines("SMOL_EXPORT")

    if is_mode("release") then
        set_optimize("fastest")
        set_strip("all")
    end
    
    add_deps("smol-interface")
    add_deps("smol-engine")

    add_files("src/**.cpp")
    add_includedirs("src")

    after_build(function (target)
        local dest_dir = target:targetdir()
        os.trycp("assets", dest_dir)
    end)
target_end()