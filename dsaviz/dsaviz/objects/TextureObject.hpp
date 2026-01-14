#pragma once
#include <dsaviz/core/VizObject.hpp>
#include <dsaviz/render/Texture.hpp>

namespace dsaviz {
class TextureObject : public VizObject {
public:
  TextureObject(Texture* texture) : texture(texture) {};
  void submit(Renderer &renderer) override;
  private:
  Texture* texture;
};
} // namespace dsaviz