#pragma once
#include <dsaviz/core/VizContext.hpp>
#include <GLFW/glfw3.h>

namespace dsaviz {

    class Input {
    public:
        Input() = default;

        void setMouseCapture(VizContext& ctx, bool enable);

        // Must be called once after window creation
        void initialize(VizContext& ctx);

        // Call once per frame (before using input)
        void beginFrame(VizContext& ctx);

        // Query helpers (inline-friendly)
        static bool keyDown(const VizContext& ctx, int key);
        static bool keyPressed(const VizContext& ctx, int key);
        static bool keyReleased(const VizContext& ctx, int key);

        static bool mouseDown(const VizContext& ctx, int button);
        static bool mousePressed(const VizContext& ctx, int button);
        static bool mouseReleased(const VizContext& ctx, int button);

    private:
        // GLFW callbacks
        static void keyCallback(
            GLFWwindow* window,
            int key, int scancode,
            int action, int mods
        );

        static void mouseButtonCallback(
            GLFWwindow* window,
            int button,
            int action,
            int mods
        );

        static void cursorPosCallback(
            GLFWwindow* window,
            double xpos,
            double ypos
        );

        static void scrollCallback(
            GLFWwindow* window,
            double xoffset,
            double yoffset
        );
    };

}
