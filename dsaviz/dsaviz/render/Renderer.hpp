#pragma once
#include <dsaviz/render/RenderCommand.hpp>
#include <dsaviz/render/CircleRenderer.hpp>
#include <dsaviz/render/SquareRenderer.hpp>
#include <dsaviz/render/TextRenderer.hpp>
#include <dsaviz/render/TextureRenderer.hpp>
#include <dsaviz/core/Camera.hpp>

namespace dsaviz {
class Renderer {
public:
  void initialize(Camera* camera);
  void submit(const RenderCommand command);
  void flush();

private:
  Camera* camera;
  std::vector<RenderCommand> opaqueCommands;
  std::vector<RenderCommand> transparentCommands;
  CircleRenderer circleRenderer;
  SquareRenderer squareRenderer;
  TextRenderer textRenderer;
  TextureRenderer textureRenderer;
};
} // namespace dsaviz