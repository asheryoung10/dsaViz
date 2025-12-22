#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <dsaViz/glfw_dark_mode.hpp>

#include <dsaViz/logSetup.hpp>
#include <dsaViz/renderTriangle.hpp>
#include <dsaViz/renderText.hpp>

#include <math.h>

void glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW error {}: {}", error, description);
}

void glfw_resize_callback(GLFWwindow* window, int width, int height)
{
    spdlog::info("GLFW window resized to {}x{}", width, height);
    glViewport(0, 0, width, height);
    dsaViz::RenderText* renderState = static_cast<dsaViz::RenderText*>(glfwGetWindowUserPointer(window));
    if (renderState) {
        spdlog::info("Re-setup render state due to window resize.");
        glClear(GL_COLOR_BUFFER_BIT);
        renderState->render();
        glfwSwapBuffers(window);
    }
}

int main()
{
    dsaViz::setupLogging(spdlog::level::warn);
    spdlog::info("Logging initialized.");

    spdlog::info("Setting glfw error callback.");
    glfwSetErrorCallback(glfw_error_callback);

    spdlog::info("Initializing GLFW.");
    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW.");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int windowWidth = mode->width * 3 / 4;
    int windowHeight = mode->height * 3 / 4;
    spdlog::info("Creating GLFW window with size {}x{}.", windowWidth, windowHeight);
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "dsaViz", nullptr, nullptr);
    if (!window) {
        spdlog::critical("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    int windowWidthActual, windowHeightActual;
    glfwGetFramebufferSize(window, &windowWidthActual, &windowHeightActual);
    glfwSetWindowPos(window,
        (mode->width - windowWidthActual) / 2,
        (mode->height - windowHeightActual) / 2
    );
    dsaViz::enableGLFWDarkMode(window); 
    spdlog::info("GLFW window created with size {}x{}.", windowWidthActual, windowHeightActual);

    spdlog::info("Making OpenGL context current.");
    glfwMakeContextCurrent(window);

    spdlog::info("Setting swap interval to 1.");
    glfwSwapInterval(1);

    spdlog::info("Initializing opengl functions with glad.");
     int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }
    spdlog::info("OpenGL initialized: {}.", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    spdlog::info("Setting GLFW framebuffer resize callback.");
    glfwSetFramebufferSizeCallback(window, glfw_resize_callback);

    dsaViz::RenderState* renderState = new dsaViz::RenderTriangle(window);
    renderState->setup();
    dsaViz::RenderText* renderTextState = new dsaViz::RenderText(window);
    renderTextState->setup(256.0f);
    glfwSetWindowUserPointer(window, renderTextState);

    spdlog::info("Starting main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        renderState->render();
        renderTextState->renderFmt(-0.9f, 0.9f, 0.2f, "Time {}", glfwGetTime());

        glfwSwapBuffers(window);
    }
    spdlog::info("Exited main loop.");

    spdlog::info("Destroying GLFW window and terminating.");
    glfwDestroyWindow(window);
    glfwTerminate();

    spdlog::info("Clean shutdown complete.");
    return 0;
}