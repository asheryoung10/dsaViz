#include <dsaviz/objects/SquareObject.hpp>

namespace dsaviz {
    void SquareObject::submit(Renderer &renderer) {
        renderer.submit(RenderCommand{
            RenderCommandType::Square,
            transform.getMatrix(),
            Color::toVec3(color)
        });
    }
}