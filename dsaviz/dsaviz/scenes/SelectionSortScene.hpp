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
        Play,
        Pause,
        Step
    };
    State state = State::Setup;
    std::vector<std::unique_ptr<ValueSquareObject>> values;


    void drawSceneUI();
    void drawSetupUI(VizContext& context);
    void resizeValues(int newSize, VizContext& context);
};
} // namespace dsaviz
