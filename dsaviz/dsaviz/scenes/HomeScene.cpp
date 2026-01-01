#include <dsaviz/scenes/HomeScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/CircleObject.hpp>
#include <dsaviz/objects/SquareObject.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

namespace dsaviz {
HomeScene::HomeScene() {
  constexpr int numCircles = 1000;
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
void sceneUI(VizContext& context)
{
    ImGuiViewport* vp = ImGui::GetMainViewport();


    ImVec2 panelPos(
        vp->WorkPos.x + vp->WorkSize.x * 0.5f,
        vp->WorkPos.y + vp->WorkSize.y - 16.0f
    );

    ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("SceneUI", nullptr, flags);

    // ── Controls row ───────────────────────
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 6));

    if (ImGui::Button("Play"))  { /* start scene */ }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) { /* pause scene */ }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))  { /* stop scene */ }
    ImGui::SameLine();
    if (ImGui::Button("Step"))  { /* stop scene */ }


    ImGui::PopStyleVar();

    ImGui::End();
}

void HomeScene::update(VizContext &context) {
  vizObjects[0]->transform.rotate(
      0.5f *
          static_cast<float>(context.frameTimeTracker->getCurrentFrameTime()),
      glm::vec3(0, 0, 1));
  for( auto& obj : vizObjects) {
    obj->transform.lookAt(context.camera->getPosition());
  }
  sceneUI(context);
}
} // namespace dsaviz