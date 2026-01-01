#pragma once
#include <dsaviz/core/VizObject.hpp>

namespace dsaviz {
class SquareObject : public VizObject {
public:
  void submit(Renderer &renderer) override;
};
} // namespace dsaviz