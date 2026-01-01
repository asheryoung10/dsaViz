cmake_minimum_required(VERSION 3.30)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(dsaviz LANGUAGES CXX C)

file(GLOB_RECURSE DSAVIZ_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/dsaviz/**/*.cpp"
)



add_executable(dsaviz
    ${DSAVIZ_SOURCES}
    ./dsaviz/main.cpp
    ./external/gl.c
)

include(FetchDeps.cmake)

target_include_directories(dsaviz PRIVATE
    ./dsaviz
    ./external/include
)

target_link_libraries(dsaviz PRIVATE
    glfw
    fmt
    spdlog
    msdf-atlas-gen
    stb_lib
    glm
    miniaudio_lib
    imgui_lib
)