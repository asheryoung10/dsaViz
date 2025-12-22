#include "glfw_dark_mode.hpp"

#if defined(_WIN32)

    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>

    #include <windows.h>
    #include <dwmapi.h>

    #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
        #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
    #endif

    #pragma comment(lib, "dwmapi.lib")

#elif defined(__APPLE__)

    #define GLFW_EXPOSE_NATIVE_COCOA
    #include <GLFW/glfw3native.h>
    #import <Cocoa/Cocoa.h>

#endif

namespace dsaViz {

void enableGLFWDarkMode(GLFWwindow* window)
{
    if (!window) return;

#if defined(_WIN32)

    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd) return;

    BOOL enable = TRUE;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &enable,
        sizeof(enable)
    );

#elif defined(__APPLE__)

    NSWindow* nsWindow = glfwGetCocoaWindow(window);
    if (!nsWindow) return;

    [nsWindow setAppearance:
        [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua]
    ];

#else
    // Linux / unsupported platforms → intentionally no-op
    (void)window;
#endif
}

}
