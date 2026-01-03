#pragma once
#include <dsaviz/render/Renderer.hpp>
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/core/VizObject.hpp>
#include <memory>

namespace dsaviz {
class Scene {
public:
  virtual void update(VizContext &context) = 0;
  virtual void render(Renderer &renderer) = 0;
  std::string getName() {return name;}
  virtual ~Scene() = default;
  protected:
  std::string name;
};
} // namespace dsaviz