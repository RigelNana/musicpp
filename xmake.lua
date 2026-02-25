set_project("music-cpp")
add_rules("mode.debug", "mode.release")
set_languages("c++20")
option("test", {showmenu = true,  default = false})
if is_plat("windows") then
    set_runtimes("MD")
    add_cxflags("/permissive-", {tools = "cl"})
end
if has_config("test") then
    add_requires("boost_ut")
end
target("music-cpp")
    set_kind("headeronly")
    add_includedirs("include", {interface = true})
    add_headerfiles("include/(musicpp/*.hpp)")
target("example")
    set_kind("binary")
    add_files("example/*.cpp")
    if is_plat("windows") then
        add_ldflags("/subsystem:console")
    end
    add_deps("music-cpp")
for _, file in ipairs(os.files("test/*_test.cpp")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        add_files(file)
        if is_plat("windows") then
            add_ldflags("/subsystem:console")
        end
        add_deps("music-cpp")
        add_packages("boost_ut")
        add_tests(name)
end