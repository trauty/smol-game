set_project("smol-game")
set_version("0.0.1")

set_config("game_name", "smol-game")

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

includes("smol-engine")

local is_standalone = has_config("standalone")

target("smol-game")
    if is_standalone then
        set_kind("static")
        if is_plat("android") then
            add_cxflags("-fPIC", {force = true})
        end
    else
        set_kind("shared")
        if is_plat("linux") then 
            add_shflags("-Wl,-Bsymbolic")
        end
    end
    
    set_basename(get_config("game_lib_name"))
    
    if is_arch("x86_64", "x64") then
        add_cxflags("-march=x86-64-v3")
    end

    set_targetdir("bin")

    if is_mode("debug") then
        if not is_plat("android") then
            set_policy("build.sanitizer.address", true)
        end

        if is_plat("windows") then
            add_defines("_DISABLE_STRING_ANNOTATION", "_DISABLE_VECTOR_ANNOTATION", {public = true})
        end
    end

    add_defines("SMOL_EXPORT")

    if is_mode("release") then
        set_optimize("fastest")
        set_strip("all")
    end
    
    add_deps("smol-interface")
    add_deps("smol-engine")

    add_files("src/**.cpp")
    add_includedirs("src")

    if not is_plat("android") then
        add_deps("smol-cooker", {inherit = false})
        after_build(function (target)
            local dest_dir = target:targetdir()

            local cooker_bin = target:dep("smol-cooker"):targetfile()
            local engine_assets = path.join(os.scriptdir(), "smol-engine/assets")
            local game_assets = "assets"
            local out_dir = ".smol"

            print("Running smol-cooker...")
            os.execv(cooker_bin, {
                "-i", engine_assets,
                "-i", game_assets,
                "-o", out_dir
            })

            local target_assets = path.join(target:dep("smol-cooker"):targetdir(), out_dir)
            os.mkdir(target_assets)
            os.cp(path.join(out_dir, "*"), target_assets)

            print("Cooked assets and copied folder to build dir: " .. target_assets)

            local trigger_file = target:targetfile() .. ".trigger"
            io.writefile(trigger_file, os.date("%Y-%m-%d %H:%M:%S"))
        end)
    end
target_end()