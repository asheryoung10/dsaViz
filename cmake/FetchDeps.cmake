include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# glad
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)


# freetype
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
)
FetchContent_MakeAvailable(freetype)


FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.16.0  # or a specific version you prefer
)

# Only populate the source — do NOT add_subdirectory or build
FetchContent_Populate(spdlog)

# Now add the include directory only
target_include_directories(dsaViz
    PRIVATE
        ${spdlog_SOURCE_DIR}/include
)


# =========================
# Asio (standalone)
# =========================
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-30-2  # pick a stable tag
)

FetchContent_MakeAvailable(asio)

# Asio does not define a CMake target, so we create one
add_library(asio INTERFACE)

target_include_directories(asio INTERFACE
  ${asio_SOURCE_DIR}/asio/include
)

target_compile_definitions(asio INTERFACE
  ASIO_STANDALONE
)

# =========================
# miniaudio
# =========================
FetchContent_Declare(
  miniaudio
  GIT_REPOSITORY https://github.com/mackron/miniaudio.git
  GIT_TAG 0.11.21   # stable release
)

FetchContent_MakeAvailable(miniaudio)

# miniaudio is a single-header library
add_library(miniaudio INTERFACE)

target_include_directories(miniaudio INTERFACE
  ${miniaudio_SOURCE_DIR}
)

# Needed for implementations in exactly one TU
target_compile_definitions(miniaudio INTERFACE
  MINIAUDIO_IMPLEMENTATION
)