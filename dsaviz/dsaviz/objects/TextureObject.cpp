#include <dsaviz/objects/TextureObject.hpp>

namespace dsaviz {
    void TextureObject::submit(Renderer &renderer) {
        renderer.submit(RenderCommand{
            RenderCommandType::Texture,
            transform.getMatrix(),
            {0,0,0},
            {0,0,0},
            texture,
            nullptr
        });
    }
}