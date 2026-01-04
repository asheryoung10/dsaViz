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
        Play,
        Pause,
        StepForward,
        StepBackward
    };
    State state = State::Setup;
    struct Animation{
        float duration;
        float elapsedTime;
        int targetIndex;
        std::vector<glm::vec3> positions;
    };
    std::vector<Animation> animations;
    std::vector<std::unique_ptr<ValueSquareObject>> values;
    float speed = 1;


    void drawSceneUI();
    void drawSetupUI(VizContext& context);
    void resizeValues(int newSize, VizContext& context);
    bool handleAnimation(Animation& animation, VizContext &context);
};
} // namespace dsaviz
