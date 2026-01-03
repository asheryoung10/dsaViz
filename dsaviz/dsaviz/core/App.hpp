#pragma once
#include <dsaviz/audio/Audio.hpp>
#include <dsaviz/core/Input.hpp>
#include <dsaviz/render/Renderer.hpp>
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>
#include <dsaviz/core/Camera.hpp>
#include <dsaviz/text/Font.hpp>
#include <dsaviz/util/Random.hpp>

namespace dsaviz {
class App {
public:
  App();
  ~App();
  int run();

private:
  void iterate();
  void appUI();
  void sceneSwitchUI();
  Audio audio;
  Camera camera;
  Input input;
  Renderer renderer;
  VizContext context;
  FrameTimeTracker frameTimeTracker;
  Font font;
  Random random;
  int sceneIndex;
  std::vector<std::unique_ptr<Scene>> scenes;
  bool shouldRun = true;
    static void glfwErrorCallback(int error, const char* description);
    static void glfwFramebufferResizeCallback(GLFWwindow* window, int width, int height);
};
} // namespace dsaviz