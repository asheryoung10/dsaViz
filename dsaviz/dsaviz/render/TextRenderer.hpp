#pragma once
#include <dsaviz/render/Texture.hpp>
#include <dsaviz/render/Shader.hpp>

namespace dsaviz {
class TextRenderer {
public:
  void initialize();

  //quadData contains x, y, width, height, u0, v0, u1, v1 for each glyph it has (can be multiple)
  void render(const glm::mat4 &transform, const glm::vec3 &color, const std::vector<float>* quadData, const Texture* fontAtlas);

private:
  unsigned int vao;
  unsigned int vbo;
  Shader shader;
};

} // namespace dsaviz