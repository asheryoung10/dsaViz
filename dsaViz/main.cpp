#include <glad/gl.h>
#include <dsaViz/logSetup.hpp>
#include <dsaViz/input.hpp>
#include <dsaViz/dsaContext.hpp>
#include <dsaViz/renderText.hpp>

#include <GLFW/glfw3.h>

void glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW error {}: {}", error, description);
}

int main()
{
    // ------------------------------------------------------------
    // Logging
    // ------------------------------------------------------------
    dsaViz::setupLogging(spdlog::level::info);
    spdlog::info("Logging initialized.");

    // ------------------------------------------------------------
    // GLFW init
    // ------------------------------------------------------------
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int windowWidth  = mode->width  * 3 / 4;
    int windowHeight = mode->height * 3 / 4;

    GLFWwindow* window =
        glfwCreateWindow(windowWidth, windowHeight, "dsaViz", nullptr, nullptr);

    if (!window) {
        spdlog::critical("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        spdlog::critical("Failed to initialize OpenGL.");
        return -1;
    }

    // ------------------------------------------------------------
    // Create context
    // ------------------------------------------------------------
    dsaViz::DSAContext ctx;
    ctx.window.handle = window;

    glfwGetFramebufferSize(
        window,
        &ctx.window.width,
        &ctx.window.height
    );

    ctx.time.now = glfwGetTime();

    // ------------------------------------------------------------
    // Input system init (registers callbacks)
    // ------------------------------------------------------------
    dsaViz::InputSystem input;
    input.initialize(ctx);

    // ------------------------------------------------------------
    // App / State
    // ------------------------------------------------------------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    dsaViz::RenderText renderText;
    renderText.setup();

    // ------------------------------------------------------------
    // Main loop
    // ------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // ---- Time ----
        double newTime = glfwGetTime();
        ctx.time.delta = newTime - ctx.time.now;
        ctx.time.now   = newTime;

        // ---- Input ----
        input.beginFrame(ctx);     // swap buffers
        glfwPollEvents();


        // ---- Render ----
        glViewport(0, 0, ctx.window.width, ctx.window.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderText.render();

        glfwSwapBuffers(window);
    }

    spdlog::info("Exited main loop.");

    // ------------------------------------------------------------
    // Shutdown
    // ------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();

    spdlog::info("Clean shutdown complete.");
    return 0;
}
