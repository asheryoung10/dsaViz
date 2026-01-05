#include <dsaviz/scenes/SelectionSortScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/CircleObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <spdlog/spdlog.h>
#include <dsaviz/util/FrameTimeTracker.hpp>

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

  ImGui::SameLine();
  if (ImGui::Button("<<")) {
    state = State::Reverse;
  }
  ImGui::SameLine();
  if (ImGui::Button("<")) {
    state = State::StepBackward;
    if(stepIndex == 0) {
      state = State::Pause;
    }
    if(stepIndex > 0) {
      stepIndex--;
      initializeStep();
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Pause")) {
    state = State::Pause;
  }
  ImGui::SameLine();
  if (ImGui::Button(">")) {
    state = State::StepForward;
 if(stepIndex < steps.size() - 1) {
    stepIndex++;
    initializeStep();
  }

  if(stepIndex == steps.size() - 1) {
    generateNextStep();
    stepIndex = steps.size() - 1;
  
  }
}
  ImGui::SameLine();
  if (ImGui::Button(">>")) {
    state = State::Play;
  }
  ImGui::SliderFloat("Speed", &speed, 0.25, 3);
  if(ImGui::SliderInt("Progress", &stepIndex, 0, steps.size() - 1)) {
    initializeStep();
    state = State::Pause;
  }
  
  if (ImGui::Button("Setup")) {
    state = State::Setup;
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    state = State::Reset;
    stepIndex = 0;
    initializeStep();
  }
  ImGui::SameLine();

  ImGui::End();
}

void SelectionSortScene::drawSetupUI(VizContext &context) {
  if (values.size() < 5) {
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
  ImGui::SliderInt("Element Count", &elementCount, 5, 1000);
  if (elementCount != values.size()) {
    resizeValues(elementCount, context);
  }

  if (ImGui::Button("Done")) {
    steps.clear();
    
    state = State::Pause;
    std::vector<ValueSquareObject*> stepValues;
    stepValues.reserve(values.size());
    for(auto& pointer : values) {
      stepValues.push_back(pointer.get());
    }
    std::vector<Animation> stepAnimations;
    float duration = 2;
    float increment = duration / float(values.size());
    std::vector<Color> fillColor;
    std::vector<Color> outlineColor;
    std::vector<Color> textColor;
    for(int i = 0; i < values.size(); i++) {
      std::vector<glm::vec3> positions;
      glm::vec3 position = values[i]->transform.getPosition();
      positions.push_back(position);
      positions.push_back(position+glm::vec3(0,1,0));
      positions.push_back(position);
      Animation animation {increment * (i+1), 0, i, positions};
      stepAnimations.push_back(animation);
      fillColor.push_back(values[i]->color);
      outlineColor.push_back({0,0,0});
      textColor.push_back({1,1,0});


    }
    stepIndex = 0;
    steps.push_back({stepValues, stepAnimations, fillColor, outlineColor, textColor, duration});
    initializeStep();

  }

  ImGui::PopStyleVar();

  ImGui::End();
}
void SelectionSortScene::initializeStep() {
  float origin = -float(steps[stepIndex].values.size())/2 + 0.5;
  for(int i = 0; i < steps[stepIndex].values.size(); i++) {
    ValueSquareObject* pointer = steps[stepIndex].values[i];
    pointer->transform.setPosition(glm::vec3(origin + i, 0, 0));
    pointer->color = steps[stepIndex].fillColors[i];
    pointer->outlineColor = steps[stepIndex].outlineColors[i];
    pointer->setTextColor(steps[stepIndex].textColors[i]);

  }
  for(int i = 0; i < steps[stepIndex].animations.size(); i++) {
    steps[stepIndex].animations[i].elapsedTime = 0;
  }
}

void SelectionSortScene::resizeValues(int newSize, VizContext &context) {
  while (newSize > values.size()) {
    std::unique_ptr addition = std::make_unique<ValueSquareObject>(
        context.random->intRange(0, 100), context);
    addition->transform.setPosition(glm::vec3(values.size(), 0, 0));
    values.push_back(std::move(addition));
  }
  while (newSize < values.size()) {
    values.resize(newSize);
  }

  float origin = -float(values.size()) / 2 + 0.5f;
  for (int i = 0; i < values.size(); i++) {
    auto &valueObject = values[i];
    int intValue = context.random->intRange(0, values.size());
    float value = intValue / float(values.size());
    valueObject->setValue(intValue);

    valueObject->color = Color::fromHSV(1, 1, value);
    valueObject->outlineColor = {1,1,1};
    valueObject->transform.setPosition(glm::vec3(origin + i, 0, 0));
  }

}
void SelectionSortScene::update(VizContext &context) {
  if (state == State::Setup) {
    drawSetupUI(context);
    return;
  }
  drawSceneUI();

  if(state == State::Pause || state == State::Reset) return;
  
  bool animationComplete = true;
  for(int i = 0; i < steps[stepIndex].animations.size(); i++) {
    if(!handleAnimation(steps[stepIndex].animations[i], context)) animationComplete = false;
  }
  
  if(!animationComplete) return;
  if(state == State::StepForward || state == State::StepBackward) {
    state = State::Pause;
    return;
  }

  if(state == State::Reverse && stepIndex == 0) {
    state = State::Pause;
    return;
  }

  if(state == State::Reverse && stepIndex > 0) {
    stepIndex--;
    initializeStep();
    return;
  }

  if(state == State::Play && stepIndex < steps.size() - 1) {
    stepIndex++;
    initializeStep();
    return;
  }

  if(state == State::Play && stepIndex == steps.size() - 1) {
    generateNextStep();
    stepIndex = steps.size() - 1;
    return;
  }
  



  // for(int i = 0; i < values.size(); i++) {
  //   int smallest = values[i]->getValue();
  //   int smallestIndex = i;
  //   for(int j = i+1; j < values.size(); j++) {
  //     if(values[j]->getValue() < smallest) {
  //       smallest = values[j]->getValue();
  //       smallestIndex= j;
  //     }
  //   }
  //   std::unique_ptr<ValueSquareObject> temp = std::move(values[i]);
  //   values[i] = std::move(values[smallestIndex]);
  //   values[smallestIndex] = std::move(temp);
  // }

  // float origin = -float(values.size())/2 + 0.5;
  // for(int i = 0; i < values.size(); i++) {
  //   values[i]->transform.setPosition(glm::vec3(origin + i, 0, 0));
  // }
}
void SelectionSortScene::generateNextStep() {

  stepIndex = steps.size() - 1;
  SelectionSortStep currentStep = steps[stepIndex];
  SelectionSortState currentState = steps[stepIndex].selectionSortState;
  if(currentState.task == SelectionSortTask::FindingMin) {
    int newMin = currentState.currentMin;
    int newMinIndex = currentState.minIndex;
    int newI = currentState.i;    
    int newJ = currentState.j+1;
    if(currentState.i == currentState.j) {
      int newMin = currentStep.values[currentState.i]->getValue();
      newMinIndex = currentState.i;
      newJ = currentState.i + 1;
    }
    if(currentState.currentMin > currentStep.values[newJ]->getValue()) {
      newMin = currentStep.values[newJ]->getValue();
      newMinIndex = newJ;
    }
    SelectionSortState newState = {
      SelectionSortTask::FindingMin,
      newMin,
      newMinIndex,
      newI,
      newJ
    };
    spdlog::info(newState.j);
    std::vector<ValueSquareObject*> newValues = currentStep.values;
        std::vector<Animation> newAnimations;
        std::vector<Color> newFillColors = currentStep.fillColors;
        std::vector<Color> newOutlineColors = currentStep.outlineColors;
        std::vector<Color> newTextColors = currentStep.textColors;
        float stepDuration = 1;
        std::vector<glm::vec3> newPositions;
        glm::vec3 currentPosition = currentStep.values[newJ]->transform.getPosition();
        newPositions.push_back(currentPosition);
        newPositions.push_back(currentPosition + glm::vec3(0,1,0));
        newPositions.push_back(currentPosition);

      Animation newAnimation { 
        stepDuration,
        0,
        newJ,
        newPositions
      };
      
      newAnimations.push_back(newAnimation);
      steps.push_back({
        newValues,
        newAnimations,
        newFillColors,
        newOutlineColors,
        newTextColors,
        stepDuration,
        newState
      });
  }
  
}
bool SelectionSortScene::handleAnimation(Animation &animation,
                                         VizContext &context) {
                                          auto easeInOut = [](float t) -> float {
                                            if(t < 0.0f) return 0.0f;
                                            if(t > 1.0f) return 1.0f;
                                            return t  * t * (3.0f - 2.0f * t);
                                          };
                                          animation.elapsedTime += context.frameTimeTracker->getCurrentFrameTime() * speed;
                                          float progress = animation.elapsedTime / animation.duration;
                                          if(animation.positions.size() == 0) return true;
                                          if(progress >=1 )  {
                                            animation.elapsedTime = animation.duration;
                                            steps[stepIndex].values[animation.targetIndex]->transform.setPosition(animation.positions[animation.positions.size() - 1]);
                                              return true;
                                          }
                                          float individualLerpProgress = 1 / (float(animation.positions.size()-1));
                                          int currentIndex = int(progress / individualLerpProgress);
                                          float currentProgress = (progress - (individualLerpProgress*currentIndex))/individualLerpProgress;
                                          currentProgress = easeInOut(currentProgress);
                                          glm::vec3 newPos = glm::mix(animation.positions[currentIndex], animation.positions[currentIndex+1], currentProgress);
                                          steps[stepIndex].values[animation.targetIndex]->transform.setPosition(newPos);
            
                                          return false;
                                         }
void SelectionSortScene::render(Renderer &render) {
  for (auto &value : values) {
    value->submit(render);
  }
}
} // namespace dsaviz
