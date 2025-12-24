#include <glad/gl.h>
#include <dsaViz/logSetup.hpp>
#include <dsaViz/input.hpp>
#include <dsaViz/dsaContext.hpp>
#include <dsaViz/renderText.hpp>
#include <dsaViz/camera.hpp>

#include <GLFW/glfw3.h>

void glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW error {}: {}", error, description);

}

void frameIteration(dsaViz::DSAContext* ctx) {
    // ---- Time ----
    double newTime = glfwGetTime();
    ctx->time.delta = newTime - ctx->time.now;
    ctx->time.now   = newTime;

    // ---- Input ----
    ctx->inputSystem->beginFrame(*ctx);     // swap buffers
    glfwPollEvents();


    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F1)) ctx->camera->setOrientation(glm::vec3(0), glm::quat(1,0,0,0));
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F2)) ctx->camera->setMode(dsaViz::CameraMode::FPS);
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F3)) ctx->camera->setMode(dsaViz::CameraMode::FreeFly);
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F4)) ctx->camera->setMode(dsaViz::CameraMode::AxisAligned);
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F5)) {
        if(ctx->window.vsync) {
            glfwSwapInterval(0);
            ctx->window.vsync = false;
        } else {
            glfwSwapInterval(1);
            ctx->window.vsync = true;
        }
    }
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_ESCAPE)) {
        bool newCaptureState = !ctx->window.mouseCaptured;
        ctx->inputSystem->setMouseCapture(*ctx, newCaptureState);
        ctx->window.mouseCaptured = newCaptureState;
    }
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_F11)) {
        GLFWwindow* window = ctx->window.handle;
        if(glfwGetWindowMonitor(window)) {
            // Currently fullscreen, switch to windowed
            glfwSetWindowMonitor(
                window,
                nullptr,
                100, 100,
                ctx->window.width,
                ctx->window.height,
                GLFW_DONT_CARE
            );
        } else {
            // Currently windowed, switch to fullscreen
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(
                window,
                monitor,
                0, 0,
                mode->width,
                mode->height,
                mode->refreshRate
            );
        }
    }
    if(ctx->inputSystem->keyDown(*ctx, GLFW_KEY_F12)) glfwSetWindowShouldClose(ctx->window.handle, GLFW_TRUE);

    
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_LEFT)) ctx->window.uiScale -= 0.1f;
    if(ctx->inputSystem->keyPressed(*ctx, GLFW_KEY_RIGHT)) ctx->window.uiScale += 0.1f;

    ctx->camera->update(*ctx);


    // ---- Render ----
    glViewport(0, 0, ctx->window.width, ctx->window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ctx->renderText->render(true);
    ctx->uiText->renderFmt(-1.0, 1.0, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "Current Time {:.2f}", glfwGetTime());
    ctx->uiText->renderFmt(-1.0, 1.0 - 0.1 * ctx->window.uiScale, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "UI Scale: {}", ctx->window.uiScale);
    ctx->uiText->renderFmt(-1.0, 1.0 - 0.2 * ctx->window.uiScale, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "FPS: {}", 1.0f / ctx->time.delta);
    ctx->uiText->renderFmt(-1.0, 1.0 - 0.3 * ctx->window.uiScale, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "VSync: {}", ctx->window.vsync ? "ON" : "OFF");
    ctx->uiText->renderFmt(-1.0, 1.0 - 0.4 * ctx->window.uiScale, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "Fullscreen: {}", glfwGetWindowMonitor(ctx->window.handle) ? "ON" : "OFF");
    ctx->uiText->renderFmt(-1.0, 1.0 - 0.5 * ctx->window.uiScale, 0.1, false, glm::vec4(0, 1.0f, 0, 1.0f), "Mouse Capture: {}", ctx->window.mouseCaptured ? "ON" : "OFF");
    ctx->uiText->render(ctx->camera->toString(), -1.0f, 1.0 - 0.6 * ctx->window.uiScale, 0.1f, false, glm::vec4(0, 1.0f, 0, 1.0f));
    
    

    glfwSwapBuffers(ctx->window.handle);
}
void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    dsaViz::DSAContext* context = (dsaViz::DSAContext*) glfwGetWindowUserPointer(window);
    if(!context) {
        spdlog::warn("glfwGetWindowUserPointer(window) not valid.");
        return;
    }
    glViewport(0, 0,width, height);
    context->window.width = width;
    context->window.height = height;
    frameIteration(context);
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
    spdlog::info("Created window.");


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
    spdlog::info("Created dsaViz context.");

    // ------------------------------------------------------------
    // Input system init (registers callbacks)
    // ------------------------------------------------------------
    dsaViz::InputSystem input;
    input.initialize(ctx);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);
    ctx.inputSystem = &input;
    spdlog::info("Setup input system.");

    // ------------------------------------------------------------
    // App / State
    // ------------------------------------------------------------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    dsaViz::RenderText renderText(&ctx, 64, "../assets/palatinolinotype_roman.ttf");
    ctx.renderText = &renderText;
    dsaViz::RenderText uiText(&ctx, 64);
    ctx.uiText = &uiText;
    spdlog::info("Text rendering setup.");

    dsaViz::Camera camera;
    ctx.camera = &camera;

    // ------------------------------------------------------------
    // Main loop
    // ------------------------------------------------------------
    spdlog::info("Starting main loop.");
    while (!glfwWindowShouldClose(window)) {
        frameIteration(&ctx);
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


