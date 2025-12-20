# dsaViz
A graphical program for visualizing common data structures and cooresponding algorithms.

## Project Summary
**dsaViz** is a work-in-progress graphical application intended to visualize common data structures and their corresponding algorithms. The long-term goal is to provide an interactive and intuitive way to explore how data structures behave and how algorithms operate in real time.

At its current stage, the application only initializes a window with a solid background color and plays a sound. No data structures or algorithm visualizations are implemented yet, but the core rendering, audio, and application infrastructure are in place and ready to be expanded.

The project is written in modern C++ and uses the following libraries:
- OpenGL
- GLFW
- GLAD (glad2)
- GLM
- miniaudio
- ASIO (standalone)
- spdlog

## Build Guide
### Prerequisites
- C++20-compatible compiler
- CMake (version 3.16 or newer recommended)
- OpenGL-capable system

### Build Instructions
1. Create a build directory:
   mkdir build
   cd build

2. Generate build files with CMake:
   cmake ..

3. Build the project:
   cmake --build .

The resulting executable will be generated inside the build directory (location may vary by platform).
