#include "RenderCommand.hpp"
#include "SquareRenderer.hpp"
#include "TextRenderer.hpp"
#include "Texture.hpp"
#include <dsaviz/render/Renderer.hpp>
#include <spdlog/spdlog.h>

namespace dsaviz {
void Renderer::submit(const RenderCommand command) {
  if (command.type == RenderCommandType::Circle ||
      command.type == RenderCommandType::Square || command.type == RenderCommandType::Texture) {
    opaqueCommands.push_back(command);
  } else {
    transparentCommands.push_back(command);
  }
}

void Renderer::initialize(Camera *camera) {
  this->camera = camera;
  circleRenderer.initialize(64, 0.05f);
  squareRenderer.initialize(16, 0.01, 0.05f);
  textRenderer.initialize();
  textureRenderer.initialize();
}
void Renderer::flush() {
  for (const RenderCommand &command : opaqueCommands) {
    switch (command.type) {
    case RenderCommandType::Circle:
      circleRenderer.render(
          camera->getViewProjectionMatrix() * command.transform, command.color, command.outlineColor);
      break;

    case RenderCommandType::Square:
      squareRenderer.render(
          camera->getViewProjectionMatrix() * command.transform, command.color, command.outlineColor);
      break;
    case RenderCommandType::Texture:
      textureRenderer.render(
          camera->getViewProjectionMatrix() * command.transform, command.glyphAtlas);
      break;
    default:
      break;
    }
  }

  for (const RenderCommand &command : transparentCommands) {

    switch (command.type) {
    case RenderCommandType::Text:
      textRenderer.render(camera->getViewProjectionMatrix() * command.transform,
                          command.color, command.glyphQuadData,
                          command.glyphAtlas);
      break;
    default:
      break;
    }
  }

  opaqueCommands.clear();
  transparentCommands.clear();
}
}; // namespace dsaviz