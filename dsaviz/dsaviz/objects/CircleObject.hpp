#pragma once
#include <dsaviz/core/VizObject.hpp>

namespace dsaviz {
class CircleObject : public VizObject {
public:
  void submit(Renderer &renderer) override;
};
} // namespace dsaviz