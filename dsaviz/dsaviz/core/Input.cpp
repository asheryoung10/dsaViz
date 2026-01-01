#include <dsaviz/core/Input.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <cstring> // memcpy

namespace dsaviz {

    void Input::initialize(VizContext& ctx) {
        GLFWwindow* window = ctx.window;

        glfwSetWindowUserPointer(window, &ctx);
        glfwGetCursorPos(window, &ctx.inputState.prevMouseX, &ctx.inputState.prevMouseY);
        ctx.inputState.mouseX = ctx.inputState.prevMouseX;
        ctx.inputState.mouseY = ctx.inputState.prevMouseY;

        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        setMouseCapture(ctx, false);
    }

    void Input::setMouseCapture(VizContext& ctx, bool enable) {
        GLFWwindow* window = ctx.window;

        if (enable) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            }
            ctx.mouseCaptured = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            }
            ctx.mouseCaptured = false;
        }
    }

    void Input::beginFrame(VizContext& ctx) {
        std::memcpy(
            ctx.inputState.prevKeys,
            ctx.inputState.keys,
            sizeof(ctx.inputState.keys)
        );

        std::memcpy(
            ctx.inputState.prevMouseButtons,
            ctx.inputState.mouseButtons,
            sizeof(ctx.inputState.mouseButtons)
        );

        ctx.inputState.prevMouseX = ctx.inputState.mouseX;
        ctx.inputState.prevMouseY = ctx.inputState.mouseY;

        ctx.inputState.scrollX = 0.0;
        ctx.inputState.scrollY = 0.0;
    }

    bool Input::keyDown(const VizContext& ctx, int key) {
        return ctx.inputState.keys[key];
    }

    bool Input::keyPressed(const VizContext& ctx, int key) {
        return ctx.inputState.keys[key] && !ctx.inputState.prevKeys[key];
    }

    bool Input::keyReleased(const VizContext& ctx, int key) {
        return !ctx.inputState.keys[key] && ctx.inputState.prevKeys[key];
    }

    bool Input::mouseDown(const VizContext& ctx, int button) {
        return ctx.inputState.mouseButtons[button];
    }

    bool Input::mousePressed(const VizContext& ctx, int button) {
        return ctx.inputState.mouseButtons[button] &&
               !ctx.inputState.prevMouseButtons[button];
    }

    bool Input::mouseReleased(const VizContext& ctx, int button) {
        return !ctx.inputState.mouseButtons[button] &&
                ctx.inputState.prevMouseButtons[button];
    }

    void Input::keyCallback(
        GLFWwindow* window,
        int key,
        int /*scancode*/,
        int action,
        int /*mods*/
    ) {
        if (key < 0 || key > GLFW_KEY_LAST) return;

        auto* ctx =
            static_cast<VizContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        if (action == GLFW_PRESS)
            ctx->inputState.keys[key] = true;
        else if (action == GLFW_RELEASE)
            ctx->inputState.keys[key] = false;
    }

    void Input::mouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int /*mods*/
    ) {
        if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;

        auto* ctx =
            static_cast<VizContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        if (action == GLFW_PRESS)
            ctx->inputState.mouseButtons[button] = true;
        else if (action == GLFW_RELEASE)
            ctx->inputState.mouseButtons[button] = false;
    }

    void Input::cursorPosCallback(
        GLFWwindow* window,
        double xpos,
        double ypos
    ) {
        auto* ctx =
            static_cast<VizContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        ctx->inputState.mouseX = xpos;
        ctx->inputState.mouseY = ypos;
    }

    void Input::scrollCallback(
        GLFWwindow* window,
        double xoffset,
        double yoffset
    ) {
        auto* ctx =
            static_cast<VizContext*>(glfwGetWindowUserPointer(window));

        if (!ctx) return;

        ctx->inputState.scrollX += xoffset;
        ctx->inputState.scrollY += yoffset;
    }

}
