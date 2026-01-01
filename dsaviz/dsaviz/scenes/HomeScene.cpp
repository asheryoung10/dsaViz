#include <dsaviz/scenes/HomeScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/CircleObject.hpp>
#include <dsaviz/objects/SquareObject.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

namespace dsaviz {
HomeScene::HomeScene() {
  constexpr int numCircles = 10000;
  for (int i = 0; i < numCircles; i++) {
    std::unique_ptr<SquareObject> circle = std::make_unique<SquareObject>();
    circle->transform.setPosition(glm::vec3(i%100, i/100, rand() % 10));

    circle->color = {static_cast<float>(rand() % 100) / 100.0f,
                     static_cast<float>(rand() % 100) / 100.0f,
                     static_cast<float>(rand() % 100) / 100.0f};
    vizObjects.push_back(std::move(circle));
  }
for (int i = 0; i < numCircles; i++) {
    std::unique_ptr<CircleObject> circle = std::make_unique<CircleObject>();
    circle->transform.setPosition(glm::vec3(i%100, i/100, rand() % 10));

    circle->color = {static_cast<float>(rand() % 100) / 100.0f,
                     static_cast<float>(rand() % 100) / 100.0f,
                     static_cast<float>(rand() % 100) / 100.0f};
    vizObjects.push_back(std::move(circle));
  }
  std::unique_ptr<SquareObject> square = std::make_unique<SquareObject>();
  square->transform.setPosition(glm::vec3(0, 0, 0));
  square->color = {1.0f, 0.0f, 0.0f};
  vizObjects.push_back(std::move(square));
}

void HomeScene::update(VizContext &context) {
  vizObjects[0]->transform.rotate(
      0.5f *
          static_cast<float>(context.frameTimeTracker->getCurrentFrameTime()),
      glm::vec3(0, 0, 1));
  for( auto& obj : vizObjects) {
    obj->transform.lookAt(context.camera->getPosition());
  }
  ImGui::Begin("Home Scene");
  ImGui::Text("Welcome to the Home Scene!");
  ImGui::End();
}
} // namespace dsaviz