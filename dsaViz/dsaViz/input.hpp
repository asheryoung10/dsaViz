#pragma once
#include <dsaViz/dsaContext.hpp>

namespace dsaViz {

    class InputSystem {
    public:
        InputSystem() = default;

        void setMouseCapture(const DSAContext& ctx, bool enable);

        // Must be called once after window creation
        void initialize(DSAContext& ctx);

        // Call once per frame (before using input)
        void beginFrame(DSAContext& ctx);

        // Query helpers (inline-friendly)
        static bool keyDown(const DSAContext& ctx, int key);
        static bool keyPressed(const DSAContext& ctx, int key);
        static bool keyReleased(const DSAContext& ctx, int key);

        static bool mouseDown(const DSAContext& ctx, int button);
        static bool mousePressed(const DSAContext& ctx, int button);
        static bool mouseReleased(const DSAContext& ctx, int button);

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
