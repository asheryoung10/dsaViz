include(FetchContent)

# fetch glfw for window management and input.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# fetch glm for graphics math.
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)

# fetch spdlog for logging.
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(spdlog)

# fetch asio for tcp and udp networking.
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)
add_library(asio_lib INTERFACE)
target_include_directories(asio_lib INTERFACE
    ${asio_SOURCE_DIR}/asio/include
)
target_compile_definitions(asio_lib INTERFACE ASIO_STANDALONE)

# fetch miniaudio for easy audio.
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.21
)
FetchContent_MakeAvailable(miniaudio)
add_library(miniaudio_lib INTERFACE)
target_include_directories(miniaudio_lib INTERFACE ${miniaudio_SOURCE_DIR})
target_compile_definitions(miniaudio_lib INTERFACE MINIAUDIO_IMPLEMENTATION)

# fetch stb for image reading/writing.
FetchContent_Declare(
    stb_lib
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb_lib)
add_library(stb_lib INTERFACE)
target_include_directories(stb_lib INTERFACE ${stb_lib_SOURCE_DIR})

# fetch freetype for msdf font rendering.
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-14-1
)
FetchContent_MakeAvailable(freetype)

# Make freetype available for msdf-atlas-gen because it runs into issues.
if (NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype INTERFACE IMPORTED)

    target_link_libraries(Freetype::Freetype
        INTERFACE freetype
    )

    target_include_directories(Freetype::Freetype
        INTERFACE
            $<TARGET_PROPERTY:freetype,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()
set(FREETYPE_FOUND TRUE CACHE BOOL "" FORCE)

# fetch msdf-atlas-gen for creating a font atlas for font rendering.
set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(MSDFGEN_USE_FREETYPE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    msdf_atlas_gen
    GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
)
FetchContent_MakeAvailable(msdf_atlas_gen)