#pragma once
#include <dsaviz/audio/Audio.hpp>
#include <dsaviz/core/Input.hpp>
#include <dsaviz/render/Renderer.hpp>
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/scenes/HomeScene.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>
#include <dsaviz/core/Camera.hpp>

namespace dsaviz {
class App {
public:
  App();
  ~App();
  int run();

private:
  void iterate();
  void appUI();
  Audio audio;
  Camera camera;
  Input input;
  Renderer renderer;
  HomeScene scene;
  VizContext context;
  FrameTimeTracker frameTimeTracker;
  bool shouldRun = true;
    static void glfwErrorCallback(int error, const char* description);
    static void glfwFramebufferResizeCallback(GLFWwindow* window, int width, int height);
};
} // namespace dsaviz