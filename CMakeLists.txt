cmake_minimum_required(VERSION 3.20)
project(project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(dsaViz
    src/main.cpp
)
include(cmake/FetchDeps.cmake)



target_include_directories(dsaViz PRIVATE
    include
)

target_link_libraries(dsaViz PRIVATE
    glfw
    glad
    freetype
    glm
    asio
    miniaudio
)

if(WIN32)
    target_link_libraries(dsaViz PRIVATE Ws2_32)
endif()
