#pragma once
#include <dsaviz/core/Scene.hpp>
#include <dsaviz/objects/ValueSquareObject.hpp>
#include <dsaviz/render/Texture.hpp>
#include <stdint.h>

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
    int width = 64;
    int height = 64;
    int iterationCount = 10;
    uint8_t* imageData = nullptr;
    float prob[4] ={1};
};
} // namespace dsaviz
