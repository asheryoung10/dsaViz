#pragma once
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/objects/ValueSquareObject.hpp>
#include <dsaviz/util/Color.hpp>

namespace dsaviz {
class InsertionSortScene : public Scene {
public:
  InsertionSortScene() { name = "Insertion Sort"; }
  void update(VizContext &context) override;
  void render(Renderer &render) override;

private:
    enum class Playback {
      Backward,
      StepBackward,
      Pause,
      StepForward,
      Forward
    };
    Playback playback;
    std::vector<std::unique_ptr<ValueSquareObject>> valueSquareObjects;
    struct Animation {
      float duration;
      float accumulatedTime;
      std::vector<glm::vec3> positions;
      int valueSquareObjectIndex;
    };
    struct ValueSquareObjectData {
      ValueSquareObject* valueSquareObject;
      glm::vec3 position;
      Color fillColor;
      Color outlineColor;
      Color textColor;
    };
    struct Step {
      std::vector<ValueSquareObjectData> valueSquareObjectsData;
      std::vector<Animation> animations;
      int sortedEndIndex;
      int currentDraggedIndex;
    };
    std::vector<Step> steps;
    int stepIndex = 0;
    enum class Case {
      Best,
      Worst,
      Random
    };
    void setupUI(VizContext& context);
    void initializeValueSquareObjects(VizContext& context, Case algorithmCase, int count);
    void playbackUI(VizContext& context);
    void initializeCurrentStep();
};
} // namespace dsaviz