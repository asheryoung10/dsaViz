#include <dsaviz/scenes/InsertionSortScene.hpp>
#include <imgui.h>
#include <dsaviz/util/Random.hpp>

namespace dsaviz {
void InsertionSortScene::update(VizContext &context) { setupUI(context); }

void InsertionSortScene::render(Renderer &render) {
  for (auto &value : valueSquareObjects) {
    value->submit(render);
  }
}

void InsertionSortScene::setupUI(VizContext &context) {
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
  int elementCount = valueSquareObjects.size();
  if (ImGui::SliderInt("Element Count", &elementCount, 2, 400)) {
    initializeValueSquareObjects(context, Case::Random, elementCount);
  }

  if (ImGui::Button("Done")) {
  }
  ImGui::SameLine();
  if (ImGui::Button("Best Case")) {
    initializeValueSquareObjects(context, Case::Best, elementCount);
  }
  ImGui::SameLine();
  if (ImGui::Button("Worst Case")) {
    initializeValueSquareObjects(context, Case::Worst, elementCount);
  }

  ImGui::PopStyleVar();

  ImGui::End();
}

void InsertionSortScene::playbackUI(VizContext &context) {}

void InsertionSortScene::initializeValueSquareObjects(VizContext &context,
                                                      Case algorithmCase,
                                                      int count) {
  if (algorithmCase == Case::Random) {
    valueSquareObjects.clear();
    valueSquareObjects.reserve(count);
    Step step;
    step.valueSquareObjectsData.reserve(count);
    step.animations.reserve(count);
    float origin = -(float)count / 2 + 0.5;
    for (int i = 0; i < count; i++) {
      int value = context.random->intRange(0, count);
      valueSquareObjects.push_back(
          std::move(std::make_unique<ValueSquareObject>(value, context)));
      ValueSquareObjectData data;
      data.valueSquareObject = valueSquareObjects[i].get();
      data.fillColor = Color::fromHSV(value / (float)count, 1, 1);
      data.outlineColor = Color::fromHSV(value / (float)count, 1, 0.5);
      data.textColor = {0, 0, 0};
      data.position = glm::vec3(origin + i, 0, 0);
      step.valueSquareObjectsData.push_back(data);
    }
    steps.clear();
    steps.push_back(step);
    initializeCurrentStep();
  }
  if (algorithmCase == Case::Best || algorithmCase == Case::Worst) {
    Step firstStep = steps[0];
   float origin = -float(firstStep.valueSquareObjectsData.size()) / 2 + 0.5f;

for (int i = 1; i < firstStep.valueSquareObjectsData.size(); i++) {
    ValueSquareObjectData key = firstStep.valueSquareObjectsData[i];
    int keyValue = key.valueSquareObject->getValue();

    int j = i - 1;
    while (j >= 0 && (algorithmCase == Case::Best ?
           keyValue < firstStep.valueSquareObjectsData[j]
                          .valueSquareObject->getValue() : keyValue > firstStep.valueSquareObjectsData[j]
                          .valueSquareObject->getValue())) {

        firstStep.valueSquareObjectsData[j + 1] =
            firstStep.valueSquareObjectsData[j];

        firstStep.valueSquareObjectsData[j + 1].position =
            glm::vec3(origin + j + 1, 0, 0);

        --j;
    }

    firstStep.valueSquareObjectsData[j + 1] = key;
    firstStep.valueSquareObjectsData[j + 1].position =
        glm::vec3(origin + j + 1, 0, 0);
}
steps.clear();
  steps.push_back(firstStep);
  initializeCurrentStep();

  }
}

void InsertionSortScene::initializeCurrentStep() {
  Step currentStep = steps[stepIndex];
  for (auto data : currentStep.valueSquareObjectsData) {
    ValueSquareObject *valueSquare = data.valueSquareObject;
    valueSquare->color = data.fillColor;
    valueSquare->outlineColor = data.outlineColor;
    valueSquare->setTextColor(data.textColor);
    valueSquare->transform.setPosition(data.position);
    valueSquare->transform.setScale(glm::vec3(0.9));
  }
}
} // namespace dsaviz