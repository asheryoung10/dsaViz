#include <glad/gl.h>
#include <dsaviz/core/App.hpp>
#include <dsaviz/util/logSetup.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace dsaviz {

void App::glfwErrorCallback(int error, const char *description) {
  spdlog::error("GLFW error {}: {}", error, description);
}

void App::glfwFramebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
  VizContext *context = (VizContext *)glfwGetWindowUserPointer(window);
  if (!context) {
    spdlog::warn("glfwGetWindowUserPointer(window) not valid.");
    return;
  }
  int framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
  context->windowWidth = width;
  context->windowHeight = height;
  context->framebufferWidth = framebufferWidth;
  context->framebufferHeight = framebufferHeight;
  context->camera->updateAspectRatio(framebufferWidth, framebufferHeight);
  context->app->iterate();
}

App::App() {
  setupLogging(spdlog::level::info);
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    spdlog::critical("Failed to initialize GLFW.");
    shouldRun = false;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int windowWidth = mode->width * 3 / 4;
  int windowHeight = mode->height * 3 / 4;

  GLFWwindow *window =
      glfwCreateWindow(windowWidth, windowHeight, "dsaviz", nullptr, nullptr);
  glfwSetWindowPos(window, mode->width / 8, mode->height / 8);

  if (!window) {
    spdlog::critical("Failed to create GLFW window.");
    glfwTerminate();
    shouldRun = false;
    return;
  }
  spdlog::info("Created window.");

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(glfwGetProcAddress)) {
    spdlog::critical("Failed to initialize OpenGL.");
    shouldRun = false;
    return;
  }

  context.app = this;
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  context.windowWidth = width;
  context.windowHeight = height;
  glfwGetFramebufferSize(window, &context.framebufferWidth, &context.framebufferHeight);
  context.framebufferWidth = context.framebufferWidth;
  context.framebufferHeight = context.framebufferHeight;
  context.window = window;
  context.input = &input;
  input.initialize(context);

  context.camera = &camera;

  context.audio = &audio;
  context.frameTimeTracker = &frameTimeTracker;
  frameTimeTracker.initialize();
  glfwSetFramebufferSizeCallback(window, glfwFramebufferResizeCallback);
  renderer.initialize(&camera);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install GLFW
                     // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  io.Fonts->AddFontFromFileTTF("../assets/JetBrainsMono-Bold.ttf", 34.0f);

  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int App::run() {
  while (!glfwWindowShouldClose(context.window) && shouldRun) {
    iterate();
  }
  return 0;
}

void App::iterate() {
  // Poll events
  input.beginFrame(context);
  glfwPollEvents();
  frameTimeTracker.update();

  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Update input
  camera.update(context);

  // Update scene
  scene.update(context);

  // Render scene
  scene.render(renderer);
  renderer.flush();

  appUI();

  // Render ImGui
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap buffers
  glfwSwapBuffers(context.window);
}

void App::appUI() {
  ImGui::Begin("Hello, world!");
  ImGui::Text("This is some useful text.");
  const glm::vec3& camPos = camera.getPosition();
  glm::vec3 camForward = camera.getForward();
  glm::vec3 camUp = camera.getUp();
  ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
  ImGui::Text("Camera Forward: (%.2f, %.2f, %.2f)", camForward.x, camForward.y, camForward.z);
  ImGui::Text("Camera Up: (%.2f, %.2f, %.2f)", camUp.x, camUp.y, camUp.z);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if(input.keyPressed(context, GLFW_KEY_ESCAPE)) {
    spdlog::info("Escape key pressed, toggling mouse capture.");
    input.setMouseCapture(context, !context.mouseCaptured);
  }
  ImGui::Text("Mouse Captured: %s", context.mouseCaptured ? "Yes" : "No");
  if(ImGui::Button("FPS Camera")) {
    camera.setMode(context, CameraMode::FPS);
  }
if(ImGui::Button("FreeFly Camera")) {
    camera.setMode(context, CameraMode::FreeFly);
  }
 if(ImGui::Button("Axis-Aligned Camera")) {
    camera.setMode(context, CameraMode::AxisAligned);
  } 
  if(ImGui::Button("Reset Camera")) {
    camera.setPosition(glm::vec3(0,0,5));
    camera.setRotation(glm::quat(glm::vec3(0,0,0)));
  }
  if(ImGui::Checkbox("Vsync", &context.vsyncEnabled)) {
    glfwSwapInterval(context.vsyncEnabled ? 1 : 0);
  }
  ImGui::End();
}

App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(context.window);
  glfwTerminate();
}
} // namespace dsaviz