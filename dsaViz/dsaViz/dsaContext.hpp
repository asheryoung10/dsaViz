#pragma once
#include <GLFW/glfw3.h>

namespace dsaViz {

    struct TimeState {
        double now = 0.0;
        double delta = 0.0;
    };

    struct WindowState {
        GLFWwindow* handle = nullptr;
        int width = 0;
        int height = 0;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        float uiScale = 0.5f; // optional, future-proof
        bool vsync = true;
        bool mouseCaptured = true;
    };

    struct InputState {
        // Keyboard
        bool keys[GLFW_KEY_LAST + 1] = {};
        bool prevKeys[GLFW_KEY_LAST + 1] = {};

        // Mouse buttons
        bool mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {};
        bool prevMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {};

        // Mouse position
        double mouseX = 0.0;
        double mouseY = 0.0;
        double prevMouseX = 0.0;
        double prevMouseY = 0.0;

        // Scroll (per-frame)
        double scrollX = 0.0;
        double scrollY = 0.0;
    };

    class RenderText;
    class InputSystem;
    class Camera;
    class AudioEngine;
    struct DSAContext {
        WindowState window;
        TimeState time;
        InputState input;
        InputSystem* inputSystem;
        RenderText* renderText;
        RenderText* uiText;
        Camera* camera;
        AudioEngine* audioEngine;
    };

}
