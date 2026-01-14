#include <dsaviz/scenes/BarnsleyFern.hpp>
#include <imgui.h>
#include <dsaviz/objects/TextureObject.hpp>
#include <dsaviz/util/Random.hpp>
#include <dsaviz/util/FrameTimeTracker.hpp>

#include <algorithm>
#include <vector>
#include <memory>

namespace dsaviz {

void BarnsleyFern::drawSceneUI() {}
void BarnsleyFern::drawSetupUI() {}
void BarnsleyFern::update(VizContext &context) {
    static bool firstTime = true;
    if(firstTime) {
        texture.createFromFile("../assets/atlas.png", true);
    }
}
void BarnsleyFern::render(Renderer &render) {
    TextureObject textureObject(&texture);
    textureObject.submit(render);
}

} // namespace dsaviz
