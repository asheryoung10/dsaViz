#include <dsaviz/scenes/MainScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/SquareObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

#include <algorithm>
#include <vector>
#include <memory>

namespace dsaviz {

void MainScene::drawSceneUI() {}
void MainScene::drawSetupUI() {}
void MainScene::update(VizContext &context) {}
void MainScene::render(Renderer &render) {
    SquareObject squareObject;
    squareObject.submit(render);
}

} // namespace dsaviz
