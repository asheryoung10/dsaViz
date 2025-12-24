#include "dsaViz/input.hpp"
#include <cstring> // memcpy

namespace dsaViz {

    void InputSystem::initialize(DSAContext& ctx) {
        GLFWwindow* window = ctx.window.handle;

        // Store context pointer for callbacks
        glfwSetWindowUserPointer(window, &ctx);
        glfwGetCursorPos(window, &ctx.input.prevMouseX, &ctx.input.prevMouseY);
        ctx.input.mouseX = ctx.input.prevMouseX;
        ctx.input.mouseY = ctx.input.prevMouseY;

        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        setMouseCapture(ctx, true);
    }

    void InputSystem::setMouseCapture(const DSAContext& ctx, bool enable) {
        // Assuming 'ctx.window' is your GLFWwindow* or you have a getter like ctx.getWindow()
        GLFWwindow* window = ctx.window.handle; 

        if (enable) {
            // Hides cursor and captures it (unlimited movement)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            // Try to enable Raw Mouse Motion (disables OS acceleration) for better FPS feel
            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            }
        } else {
            // Restores normal cursor behavior
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            // Disable raw motion when in UI mode
            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            }
        }
    }

    void InputSystem::beginFrame(DSAContext& ctx) {
        // Copy current → previous
        std::memcpy(
            ctx.input.prevKeys,
            ctx.input.keys,
            sizeof(ctx.input.keys)
        );

        std::memcpy(
            ctx.input.prevMouseButtons,
            ctx.input.mouseButtons,
            sizeof(ctx.input.mouseButtons)
        );

        ctx.input.prevMouseX = ctx.input.mouseX;
        ctx.input.prevMouseY = ctx.input.mouseY;

        // Scroll is per-frame
        ctx.input.scrollX = 0.0;
        ctx.input.scrollY = 0.0;
    }

    // ======================
    // Query helpers
    // ======================

    bool InputSystem::keyDown(const DSAContext& ctx, int key) {
        return ctx.input.keys[key];
    }

    bool InputSystem::keyPressed(const DSAContext& ctx, int key) {
        return ctx.input.keys[key] && !ctx.input.prevKeys[key];
    }

    bool InputSystem::keyReleased(const DSAContext& ctx, int key) {
        return !ctx.input.keys[key] && ctx.input.prevKeys[key];
    }

    bool InputSystem::mouseDown(const DSAContext& ctx, int button) {
        return ctx.input.mouseButtons[button];
    }

    bool InputSystem::mousePressed(const DSAContext& ctx, int button) {
        return ctx.input.mouseButtons[button] &&
               !ctx.input.prevMouseButtons[button];
    }

    bool InputSystem::mouseReleased(const DSAContext& ctx, int button) {
        return !ctx.input.mouseButtons[button] &&
                ctx.input.prevMouseButtons[button];
    }

    // ======================
    // GLFW callbacks
    // ======================

    void InputSystem::keyCallback(
        GLFWwindow* window,
        int key,
        int /*scancode*/,
        int action,
        int /*mods*/
    ) {
        if (key < 0 || key > GLFW_KEY_LAST) return;

        auto* ctx =
            static_cast<DSAContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        if (action == GLFW_PRESS)
            ctx->input.keys[key] = true;
        else if (action == GLFW_RELEASE)
            ctx->input.keys[key] = false;
    }

    void InputSystem::mouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int /*mods*/
    ) {
        if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;

        auto* ctx =
            static_cast<DSAContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        if (action == GLFW_PRESS)
            ctx->input.mouseButtons[button] = true;
        else if (action == GLFW_RELEASE)
            ctx->input.mouseButtons[button] = false;
    }

    void InputSystem::cursorPosCallback(
        GLFWwindow* window,
        double xpos,
        double ypos
    ) {
        auto* ctx =
            static_cast<DSAContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        ctx->input.mouseX = xpos;
        ctx->input.mouseY = ypos;
    }

    void InputSystem::scrollCallback(
        GLFWwindow* window,
        double xoffset,
        double yoffset
    ) {
        auto* ctx =
            static_cast<DSAContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        ctx->input.scrollX += xoffset;
        ctx->input.scrollY += yoffset;
    }

}
