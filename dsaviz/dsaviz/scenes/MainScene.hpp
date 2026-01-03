#pragma once
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/objects/ValueSquareObject.hpp>

#include <vector>

namespace dsaviz {

class MainScene : public Scene {
public:
    MainScene() {
        name = "Main";
    }
    void update(VizContext& context) override;
    void render(Renderer& render) override;
private:
    void drawSceneUI();
    void drawSetupUI();
};
} // namespace dsaviz
