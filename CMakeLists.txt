cmake_minimum_required(VERSION 3.20)
project(dsaViz LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dsaViz
    dsaViz/main.cpp
    dsaViz/input.cpp
    dsaViz/shaderProgram.cpp
    dsaViz/texture.cpp
    dsaViz/renderText.cpp

    external/src/gl.c
)

# Fetch all dependencies.
include(cmake/FetchDeps.cmake)

# Put ./dsaViz in the include path.
target_include_directories(dsaViz PRIVATE
    ./dsaViz
    ./external/include
)

# Link all dependencies.
target_link_libraries(dsaViz PRIVATE
    glfw
    freetype
    glm
    asio_lib
    miniaudio_lib
    msdf-atlas-gen::msdf-atlas-gen
    spdlog::spdlog
    stb_lib
)

# Platform-specific settings.
if(WIN32)
    target_link_libraries(dsaViz PRIVATE Ws2_32)
    target_link_libraries(dsaViz PRIVATE dwmapi)
endif()
