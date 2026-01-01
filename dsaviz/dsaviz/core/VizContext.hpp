#pragma once
#include <GLFW/glfw3.h>

namespace dsaviz {
    class App;
    class Input;
    class Audio;
    class Camera;
    class FrameTimeTracker;
    struct InputState {
        bool keys[GLFW_KEY_LAST + 1] = {};
        bool prevKeys[GLFW_KEY_LAST + 1] = {};

        bool mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {};
        bool prevMouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {};

        double mouseX = 0.0;
        double mouseY = 0.0;
        double prevMouseX = 0.0;
        double prevMouseY = 0.0;

        double scrollX = 0.0;
        double scrollY = 0.0;
    };
    struct VizContext {
        App* app;
        InputState inputState;
        Input* input;
        Audio* audio;
        Camera* camera;
        FrameTimeTracker* frameTimeTracker;
        GLFWwindow* window;
        int framebufferWidth;
        int framebufferHeight;
        int windowWidth;
        int windowHeight;
        bool mouseCaptured = true;
        bool vsyncEnabled = true;
    };
}