#pragma once
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/objects/ValueSquareObject.hpp>

#include <vector>

namespace dsaviz {

class SelectionSortScene : public Scene {
public:
    SelectionSortScene() {
        name = "Selection Sort";
    }
    void update(VizContext& context) override;
    void render(Renderer& render) override;
private:
    enum class State{
        Setup,
        Reset,
        StepBackward,
        Reverse,
        Pause,
        Play,
        StepForward,
    };
    State state = State::Setup;
    struct Animation{
        float duration;
        float elapsedTime;
        int targetIndex;
        std::vector<glm::vec3> positions;
    };
    std::vector<std::unique_ptr<ValueSquareObject>> values;
;
    enum class SelectionSortTask {
        FindingMin,
        Swapping
    };
    struct SelectionSortState {
        SelectionSortTask task = SelectionSortTask::FindingMin;
        int currentMin = 0;
        int minIndex = 0;
        int i = 0;
        int j = 0;
    };
    struct SelectionSortStep {
        std::vector<ValueSquareObject*> values;
        std::vector<Animation> animations;
        std::vector<Color> fillColors;
        std::vector<Color> outlineColors;
        std::vector<Color> textColors;
        float stepDuration;
        SelectionSortState selectionSortState;
    };
    std::vector<SelectionSortStep> steps;
    int stepIndex;
    float speed = 1;


    void drawSceneUI();
    void initializeStep();
    void generateNextStep();
    void drawSetupUI(VizContext& context);
    void resizeValues(int newSize, VizContext& context);
    bool handleAnimation(Animation& animation, VizContext &context);
};
} // namespace dsaviz
