#include "SquareRenderer.hpp"
#include <dsaviz/render/Renderer.hpp>
#include <spdlog/spdlog.h>

namespace dsaviz {
void Renderer::submit(const RenderCommand command) {
  if (command.type == RenderCommandType::Circle ||
      command.type == RenderCommandType::Square) {
    opaqueCommands.push_back(command);
  } else {
    transparentCommands.push_back(command);
  }
}

void Renderer::initialize(Camera* camera) {
  this->camera = camera;
  circleRenderer.initialize(64, 0.05f);
  squareRenderer.initialize(5, 0.01, 0.05f);
  //texturedQuadsRenderer.initialize();
}
void Renderer::flush() {
    for (const RenderCommand& command : opaqueCommands) {
        switch (command.type) {
        case RenderCommandType::Circle:
            circleRenderer.render(camera->getViewProjectionMatrix() * command.transform, command.color);
            break;

        case RenderCommandType::Square:
            squareRenderer.render(camera->getViewProjectionMatrix() * command.transform, command.color);
            break;

        default:
            break;
        }
    }

    opaqueCommands.clear();
    transparentCommands.clear();
}
}; // namespace dsaviz