#include <dsaviz/scenes/SelectionSortScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/CircleObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <vector>
#include <memory>

namespace dsaviz {
void SelectionSortScene::drawSceneUI() {
ImGuiViewport *vp = ImGui::GetMainViewport();

  ImVec2 panelPos(vp->WorkPos.x + vp->WorkSize.x * 0.5f,
                  vp->WorkPos.y + vp->WorkSize.y - 16.0f);

  ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing;
  ImGui::Begin("SelectionSortControlUI", nullptr, flags);

      if (ImGui::Button("Reset")) {
        state = State::Setup;
    }
    ImGui::SameLine();
    ImGui::SameLine();
    if (ImGui::Button(">")) { /* pause scene */
    }


  ImGui::End();
}

void SelectionSortScene::drawSetupUI(VizContext &context) {
  if(values.size() < 5) {
    resizeValues(5, context);
  }
  ImGuiViewport *vp = ImGui::GetMainViewport();

  ImVec2 panelPos(vp->WorkPos.x + vp->WorkSize.x * 0.5f,
                  vp->WorkPos.y + vp->WorkSize.y - 16.0f);

  ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing;
  ImGui::Begin("SelectionSortSetupUI", nullptr, flags);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 6));
  int elementCount = values.size();
  ImGui::SliderInt("Element Count", &elementCount, 5, 50);
   if(elementCount != values.size()) {
  resizeValues(elementCount, context);
  }


  if (ImGui::Button("Done")) {
    state = State::Pause;
  }

  ImGui::PopStyleVar();

  ImGui::End();
}
void SelectionSortScene::resizeValues(int newSize, VizContext& context) {
     while (newSize > values.size()) {
    std::unique_ptr addition = std::make_unique<ValueSquareObject>(
        context.random->intRange(0, 100), context);
    addition->transform.setPosition(glm::vec3(values.size(), 0, 0));
    values.push_back(std::move(addition));
  }
  while (newSize < values.size()) {
    values.resize(newSize);
  }

  float origin = -float(values.size()) /2 + 0.5f ;
  for(int i = 0; i < values.size(); i++) {
    auto& valueObject = values[i];
    int intValue = context.random->intRange(0,values.size());
    float value =  intValue / float(values.size());
    valueObject->setValue(intValue);

    valueObject->color = Color::fromHSV(1,1,value);
    valueObject->transform.setPosition(glm::vec3(origin + i, 0, 0));
  }

}
void SelectionSortScene::update(VizContext &context) {
  if (state == State::Setup) {
    drawSetupUI(context);
    return;
  }
  drawSceneUI();
}
void SelectionSortScene::render(Renderer &render) {
  for (auto& value : values) {
    value->submit(render);
  }
}
} // namespace dsaviz
