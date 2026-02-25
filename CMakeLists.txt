cmake_minimum_required(VERSION 3.20)
project(music-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Header-only library ─────────────────────────────
add_library(musicpp INTERFACE)
target_include_directories(musicpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# MSVC-specific flags
if(MSVC)
    target_compile_options(musicpp INTERFACE /permissive-)
endif()

# ── Example ──────────────────────────────────────────
option(MUSICPP_BUILD_EXAMPLE "Build example program" ON)
if(MUSICPP_BUILD_EXAMPLE)
    add_executable(example example/song.cpp)
    target_link_libraries(example PRIVATE musicpp)
endif()

# ── Tests ────────────────────────────────────────────
option(MUSICPP_BUILD_TESTS "Build unit tests" OFF)
if(MUSICPP_BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/boost-ext/ut.git
        GIT_TAG        v2.3.1
    )
    FetchContent_MakeAvailable(ut)

    file(GLOB TEST_SOURCES test/*_test.cpp)
    foreach(test_file ${TEST_SOURCES})
        get_filename_component(test_name ${test_file} NAME_WE)
        add_executable(${test_name} ${test_file})
        target_link_libraries(${test_name} PRIVATE musicpp Boost::ut)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()

# ── Install ──────────────────────────────────────────
include(GNUInstallDirs)
install(DIRECTORY include/musicpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
