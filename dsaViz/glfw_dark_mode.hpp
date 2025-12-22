#pragma once

#include <GLFW/glfw3.h>

namespace dsaViz {

/// @brief Attempts to enable OS dark mode for a GLFW window.
/// @note This is best-effort:
/// - Windows: enables dark title bar (Win10 1809+, Win11)
/// - macOS: requires Cocoa (Objective-C++)
/// - Linux: no-op
void enableGLFWDarkMode(GLFWwindow* window);

}
