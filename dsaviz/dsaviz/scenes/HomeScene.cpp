#include <dsaviz/scenes/HomeScene.hpp>
#include <imgui.h>
#include <dsaviz/objects/ValueSquareObject.hpp>
#include <dsaviz/objects/CircleObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

#include <algorithm>
#include <vector>
#include <memory>

namespace dsaviz {
void HomeScene::drawSceneUI() {}
void HomeScene::drawSetupUI() {}
void HomeScene::update(VizContext &context) {}
void HomeScene::render(Renderer &render) {
    CircleObject circle;
    circle.submit(render);
}
} // namespace dsaviz
