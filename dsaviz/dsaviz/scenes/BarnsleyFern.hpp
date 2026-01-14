#pragma once
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/objects/ValueSquareObject.hpp>
#include <dsaviz/render/Texture.hpp>

#include <vector>

namespace dsaviz {

class BarnsleyFern : public Scene {
public:
    BarnsleyFern() {
        name = "BarnsleyFern";
    }
    void update(VizContext& context) override;
    void render(Renderer& render) override;
private:
    void drawSceneUI();
    void drawSetupUI();
    Texture texture;
};
} // namespace dsaviz
