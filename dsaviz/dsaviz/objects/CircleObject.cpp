#include <dsaviz/objects/CircleObject.hpp>

namespace dsaviz {
    void CircleObject::submit(Renderer &renderer) {
        renderer.submit(RenderCommand{
            RenderCommandType::Circle,
            transform.getMatrix(),
            Color::toVec3(color),
            Color::toVec3(outlineColor)
        });
    }
}