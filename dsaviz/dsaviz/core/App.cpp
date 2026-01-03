#include <glad/gl.h>
#include <dsaviz/core/App.hpp>
#include <dsaviz/util/logSetup.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <dsaviz/scenes/HomeScene.hpp>
#include <dsaviz/scenes/MainScene.hpp>

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

void SetupModernImGuiStyle() {
  ImGuiStyle &style = ImGui::GetStyle();

  // Layout
  style.WindowPadding = ImVec2(14, 14);
  style.FramePadding = ImVec2(10, 6);
  style.ItemSpacing = ImVec2(12, 8);
  style.ScrollbarSize = 12.0f;
  style.GrabMinSize = 10.0f;

  // Rounding
  style.WindowRounding = 8.0f;
  style.FrameRounding = 6.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabRounding = 6.0f;
  style.TabRounding = 6.0f;

  // Borders
  style.WindowBorderSize = 1.0f;
  style.FrameBorderSize = 0.0f;

  // Colors (dark modern)
  ImVec4 *c = style.Colors;
  c[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.105f, 0.11f, 0.96f);
  c[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.23f, 0.60f);

  c[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
  c[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

  c[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
  c[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
  c[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);

  c[ImGuiCol_Button] = ImVec4(0.22f, 0.23f, 0.27f, 1.00f);
  c[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
  c[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.60f, 0.90f, 1.00f);

  c[ImGuiCol_Header] = ImVec4(0.22f, 0.23f, 0.27f, 1.00f);
  c[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
  c[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.60f, 0.90f, 1.00f);

  c[ImGuiCol_SliderGrab] = ImVec4(0.18f, 0.60f, 0.90f, 1.00f);
  c[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.65f, 0.95f, 1.00f);

  c[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.60f, 0.90f, 1.00f);
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
  glfwGetFramebufferSize(window, &context.framebufferWidth,
                         &context.framebufferHeight);
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

  font.setFont("../assets/palatinolinotype_roman.ttf", 64.0f);
  context.font = &font;

  renderer.initialize(&camera);

  context.random = &random;

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
  SetupModernImGuiStyle();

  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  scenes.push_back(std::make_unique<HomeScene>());
  scenes.push_back(std::make_unique<MainScene>());
  sceneIndex = 0;
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
  ImGui::GetIO().FontGlobalScale = context.uiScale;
  frameTimeTracker.update();

  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Update input
  camera.update(context);

  // Update scene
  scenes[sceneIndex]->update(context);

  // Render scene
  scenes[sceneIndex]->render(renderer);
  renderer.flush();

  appUI();
  sceneSwitchUI();

  // Render ImGui
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap buffers
  glfwSwapBuffers(context.window);
}

void App::appUI() {
  ImGuiViewport *vp = ImGui::GetMainViewport();

  ImVec2 panelPos(vp->WorkPos.x + 16.0f, vp->WorkPos.y + 16.0f);

  ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_AlwaysAutoResize |
                           ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Info", nullptr, flags);

  ImGui::Text("Application");
  ImGui::Separator();

  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::Text("Frame: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

  ImGui::Spacing();

  if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
    const glm::vec3 &p = camera.getPosition();
    const glm::vec3 f = camera.getForward();
    const glm::vec3 u = camera.getUp();

    ImGui::Text("Position");
    ImGui::DragFloat3("##CameraPosition", (float *)&p, 0.1f);

    ImGui::Text("Forward");
    ImGui::DragFloat3("##forward", (float *)&f, 0.0f, 0.0f, 0.0f, "%.2f");

    ImGui::Text("Up");
    ImGui::DragFloat3("##up", (float *)&u, 0.0f, 0.0f, 0.0f, "%.2f");

    ImGui::Text("Camera Speed");
    ImGui::SliderFloat("##CameraSpeed", &camera.moveSpeed, 0.1f, 50.0f);
  }

  // ── Camera Modes ───────────────────────
  if (ImGui::CollapsingHeader("Camera Mode", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("FPS"))
      camera.setMode(context, CameraMode::FPS);
    ImGui::SameLine();
    if (ImGui::Button("FreeFly"))
      camera.setMode(context, CameraMode::FreeFly);
    ImGui::SameLine();
    if (ImGui::Button("Axis"))
      camera.setMode(context, CameraMode::AxisAligned);

    if (ImGui::Button("Reset Camera")) {
      camera.setPosition(glm::vec3(0, 0, 5));
      camera.setRotation(glm::quat(glm::vec3(0)));
    }
  }

  // ── System ─────────────────────────────
  if (ImGui::CollapsingHeader("System", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Checkbox("VSync", &context.vsyncEnabled))
      glfwSwapInterval(context.vsyncEnabled ? 1 : 0);

    ImGui::Text("Mouse Captured: %s", context.mouseCaptured ? "Yes" : "No");

    if (input.keyPressed(context, GLFW_KEY_ESCAPE)) {
      input.setMouseCapture(context, !context.mouseCaptured);
    }
    if (ImGui::CollapsingHeader("UI", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("UI Scale");
      ImGui::SliderFloat("##UIScale", &context.uiScale, 0.25f, 1.5f, "%.2fx");
    }
  }

  ImGui::End();
}

void App::sceneSwitchUI() 
  {
    ImGuiViewport *vp = ImGui::GetMainViewport();

    ImVec2 panelPos(vp->WorkPos.x + vp->WorkSize.x * 0.5f,
                    vp->WorkPos.y + 16.0f);

    ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always, ImVec2(0.5f, 0.0f));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::Begin("SceneSwitchUI", nullptr, flags);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 6));
    if (ImGui::Button("<")) { /* start scene */
      sceneIndex++;
      sceneIndex %= scenes.size();
      audio.playFile("../assets/subtleClick.mp3");
    }
    ImGui::SameLine();
    ImGui::Text("%s", scenes[sceneIndex]->getName().c_str());
    ImGui::SameLine();
    if (ImGui::Button(">")) { /* pause scene */
      audio.playFile("../assets/subtleClick.mp3");
      sceneIndex--;
      sceneIndex %= scenes.size();
    }

    ImGui::PopStyleVar();

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